#include <spdlog/spdlog.h>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include "MYSQL.h"
#include "RequestHandlers/request_handler.h"
#include "db_record.h"

#include "workflow/WFHttpServer.h"
#include "workflow/HttpMessage.h"
#include "workflow/HttpUtil.h"
#include "workflow/WFTaskFactory.h"
#include "workflow/WFFacilities.h"
#include "workflow/Workflow.h"

#include "workflow/MySQLResult.h"
#include "workflow/WFFacilities.h"

#include <fmt/format.h>
#include "co/flag.h"
#include "MYSQL.h"
#include "co/log.h"
#include "co/cout.h"
struct ServerConfig {
  constexpr static uint16_t port_num = 443;
  //static inline auto const ip_address = tcp::v4();
};

static WFFacilities::WaitGroup wait_group(1);


void process(WFHttpTask* task) {
  task->get_req()->get_method();
  task->get_resp()->append_output_body("<html>Hello World!</html>");
  auto* resp = task->get_resp();
  resp->add_header_pair("Content-Type", "text/html");
}

#define RETRY_MAX       0
using namespace protocol;

volatile bool stop_flag;

void mysql_callback(WFMySQLTask *task);

void get_next_cmd(WFMySQLTask *task)
{
  int len;
  char query[4096];
  WFMySQLTask *next_task;

  fprintf(stderr, "mysql> ");
  while ((fgets(query, 4096, stdin)) && stop_flag == false)
  {
    len = strlen(query);
    if (len > 0 && query[len - 1] == '\n')
      query[len - 1] = '\0';

    if (strncmp(query, "quit", len) == 0 ||
        strncmp(query, "exit", len) == 0)
    {
      fprintf(stderr, "Bye\n");
      return;
    }

    if (len == 0 || strncmp(query, "\0", len) == 0)
    {
      fprintf(stderr, "mysql> ");
      continue;
    }

    std::string *url = (std::string *)series_of(task)->get_context();
    next_task = WFTaskFactory::create_mysql_task(*url, RETRY_MAX, mysql_callback);
    next_task->get_req()->set_query(query);
    series_of(task)->push_back(next_task);
    break;
  }
  return;
}

void mysql_callback(WFMySQLTask *task)
{
  MySQLResponse *resp = task->get_resp();

  MySQLResultCursor cursor(resp);
  const MySQLField *const *fields;
  std::vector<MySQLCell> arr;

  cout << "STATE STATE : " << task->get_state() << endl;
  if (task->get_state() != WFT_STATE_SUCCESS)
  {
    fprintf(stderr, "error msg: %s\n",
            WFGlobal::get_error_string(task->get_state(),
                                       task->get_error()));
    return;
  }

  do {
    if (cursor.get_cursor_status() != MYSQL_STATUS_GET_RESULT &&
        cursor.get_cursor_status() != MYSQL_STATUS_OK)
    {
      break;
    }

    fprintf(stderr, "---------------- RESULT SET ----------------\n");

    if (cursor.get_cursor_status() == MYSQL_STATUS_GET_RESULT)
    {
      fprintf(stderr, "cursor_status=%d field_count=%u rows_count=%u\n",
              cursor.get_cursor_status(), cursor.get_field_count(),
              cursor.get_rows_count());

      //nocopy api
      fields = cursor.fetch_fields();
      for (int i = 0; i < cursor.get_field_count(); i++)
      {
        if (i == 0)
        {
          fprintf(stderr, "db=%s table=%s\n",
                  fields[i]->get_db().c_str(), fields[i]->get_table().c_str());
          fprintf(stderr, "  ---------- COLUMNS ----------\n");
        }
        fprintf(stderr, "  name[%s] type[%s]\n",
                fields[i]->get_name().c_str(),
                datatype2str(fields[i]->get_data_type()));
      }
      fprintf(stderr, "  _________ COLUMNS END _________\n\n");

      while (cursor.fetch_row(arr))
      {
        fprintf(stderr, "  ------------ ROW ------------\n");
        for (size_t i = 0; i < arr.size(); i++)
        {
          fprintf(stderr, "  [%s][%s]", fields[i]->get_name().c_str(),
                  datatype2str(arr[i].get_data_type()));
          if (arr[i].is_string())
          {
            std::string res = arr[i].as_string();
            if (res.length() == 0)
              fprintf(stderr, "[\"\"]\n");
            else
              fprintf(stderr, "[%s]\n", res.c_str());
          } else if (arr[i].is_int()) {
            fprintf(stderr, "[%d]\n", arr[i].as_int());
          } else if (arr[i].is_ulonglong()) {
            fprintf(stderr, "[%llu]\n", arr[i].as_ulonglong());
          } else if (arr[i].is_float()) {
            const void *ptr;
            size_t len;
            int data_type;
            arr[i].get_cell_nocopy(&ptr, &len, &data_type);
            size_t pos;
            for (pos = 0; pos < len; pos++)
              if (*((const char *)ptr + pos) == '.')
                break;
            if (pos != len)
              pos = len - pos - 1;
            else
              pos = 0;
            fprintf(stderr, "[%.*f]\n", (int)pos, arr[i].as_float());
          } else if (arr[i].is_double()) {
            const void *ptr;
            size_t len;
            int data_type;
            arr[i].get_cell_nocopy(&ptr, &len, &data_type);
            size_t pos;
            for (pos = 0; pos < len; pos++)
              if (*((const char *)ptr + pos) == '.')
                break;
            if (pos != len)
              pos = len - pos - 1;
            else
              pos= 0;
            fprintf(stderr, "[%.*lf]\n", (int)pos, arr[i].as_double());
          } else if (arr[i].is_date()) {
            fprintf(stderr, "[%s]\n", arr[i].as_string().c_str());
          } else if (arr[i].is_time()) {
            fprintf(stderr, "[%s]\n", arr[i].as_string().c_str());
          } else if (arr[i].is_datetime()) {
            fprintf(stderr, "[%s]\n", arr[i].as_string().c_str());
          } else if (arr[i].is_null()) {
            fprintf(stderr, "[NULL]\n");
          } else {
            std::string res = arr[i].as_binary_string();
            if (res.length() == 0)
              fprintf(stderr, "[\"\"]\n");
            else
              fprintf(stderr, "[%s]\n", res.c_str());
          }
        }
        fprintf(stderr, "  __________ ROW END __________\n");
      }
    }
    else if (cursor.get_cursor_status() == MYSQL_STATUS_OK)
    {
      fprintf(stderr, "  OK. %llu ", cursor.get_affected_rows());
      if (cursor.get_affected_rows() == 1)
        fprintf(stderr, "row ");
      else
        fprintf(stderr, "rows ");
      fprintf(stderr, "affected. %d warnings. insert_id=%llu. %s\n",
              cursor.get_warnings(), cursor.get_insert_id(),
              cursor.get_info().c_str());
    }

    fprintf(stderr, "________________ RESULT SET END ________________\n\n");
  } while (cursor.next_result_set());


  if (resp->get_packet_type() == MYSQL_PACKET_ERROR)
  {
    fprintf(stderr, "ERROR. error_code=%d %s\n",
            task->get_resp()->get_error_code(),
            task->get_resp()->get_error_msg().c_str());
  }
  else if (resp->get_packet_type() == MYSQL_PACKET_OK) // just check origin APIs
  {
    fprintf(stderr, "OK. %llu ", task->get_resp()->get_affected_rows());
    if (task->get_resp()->get_affected_rows() == 1)
      fprintf(stderr, "row ");
    else
      fprintf(stderr, "rows ");
    fprintf(stderr, "affected. %d warnings. insert_id=%llu. %s\n",
            task->get_resp()->get_warnings(),
            task->get_resp()->get_last_insert_id(),
            task->get_resp()->get_info().c_str());
  }

  get_next_cmd(task);
  return;
}

DEF_uint32(dbPort, 3306, "dbPort");
DEF_string(dbName, "db", "dbName");
DEF_string(dbUser, "root", "dbUser");
DEF_string(dbPassword, "root", "dbPassword");
DEF_string(dbHost, "localhost", "dbHost");



int main(int argc, char** argv) try {
  db::MYSQL connector{};

  flag::parse(argc, argv);
  cout << FLG_dbName << endl;

  std::string dbURL = fmt::format("mysql://{}:{}@{}:{}/{}", FLG_dbUser.data(),
                                  FLG_dbPassword.data(), FLG_dbHost.data(), FLG_dbPort, FLG_dbName.data());

  cout << dbURL << endl;
  int res = connector.init(dbURL);
  cout << "RES: " << res << endl;
  //connector.read();
  connector.create(1);
  DLOG << "db url: " << dbURL;

  for (;;)
  {}

//  WFMySQLTask *task;
//
//  auto url = "mysql://root:root@localhost:3306/db?\n";
//
//  const char *query = "show tables";
//  stop_flag = false;
//
//  task = WFTaskFactory::create_mysql_task(url, RETRY_MAX, mysql_callback);
//  task->get_req()->set_query(query);
//
//  task->start();
//  WFFacilities::WaitGroup wait_group(1);
//  SeriesWork *series = Workflow::create_series_work(task,
//                                                    [&wait_group](const SeriesWork *series) {
//                                                      wait_group.done();
//                                                    });
//
//  series->set_context(&url);
//  series->start();
//
//  wait_group.wait();
//
//  WFHttpServer server(process);
//
//  if (server.start(8888) == 0) {  // start server on port 8888
//    getchar();                    // press "Enter" to end.
//  }
//  WFGlobal::new_ssl_server_ctx();
////  auto url = "https://github.com/sogou/workflow/blob/master/tutorial/tutorial-01-wget.cc";
////  WFHttpTask *task = WFTaskFactory::create_http_task(url, 1, 1, wget_callback);
////  protocol::HttpRequest *req = task->get_req();
////  req->add_header_pair("Accept", "*/*");
////  req->add_header_pair("User-Agent", "Wget/1.14 (gnu-linux)");
////  req->add_header_pair("Connection", "close");
////  task->start();
//
//  wait_group.wait();
//
//  pause();
  return 0;
} catch (std::exception const& e) {
  ELOG << e.what();
} catch (...) {
  ELOG << "Unknown exception";
}
