#include "RequestHandlers/request_handler.h"
#include "shortened_url.h"

#include "workflow/WFHttpServer.h"
#include "workflow/HttpMessage.h"
#include "workflow/HttpUtil.h"
#include "workflow/WFTaskFactory.h"
#include "workflow/WFFacilities.h"
#include "workflow/Workflow.h"

#include "workflow/MySQLResult.h"
#include "workflow/WFFacilities.h"

#include <format>
#include "co/flag.h"
#include "co/log.h"
#include "co/cout.h"

#include "co/god.h"
#include "urls_table_handler.h"

#include <thread>
#include "config.h"

struct ServerConfig {
  constexpr static uint16_t port_num = 443;
  //static inline auto const ip_address = tcp::v4();
};

static WFFacilities::WaitGroup wait_group(1);


void handle_request(WFHttpTask* task) {
  protocol::HttpRequest* req = task->get_req();
  protocol::HttpResponse* resp = task->get_resp();
  resp->set_http_version("HTTP/1.1");

  const char* request_uri = req->get_request_uri();

  request::handle_func handler = request::Router::get_handler(request_uri);

  if (handler == nullptr) [[unlikely]] {
    resp->append_output_body_nocopy(request::default_messages::UNSUPPORTED_URI);
    protocol::HttpUtil::set_response_status(resp, HttpStatusNotImplemented);
    return;
  }

  handler(task->get_req(), task->get_resp());
}

#define RETRY_MAX       0
using namespace protocol;

volatile bool stop_flag;



DEF_uint32(dbPort, 3306, "dbPort");
DEF_string(dbName, "db", "dbName");
DEF_string(dbUser, "root", "dbUser");
DEF_string(dbPassword, "root", "dbPassword");
DEF_string(dbHost, "localhost", "dbHost");



int main(int argc, char** argv) try {
  god::bless_no_bugs();
  db::UrlsTableHandler table_handler{};
  table_handler.create_required_table();

  flag::parse(argc, argv);
  cout << FLG_dbName << endl;

  Config::dbUrl = std::format("mysql://{}:{}@{}:{}/{}", FLG_dbUser.data(),
                                  FLG_dbPassword.data(), FLG_dbHost.data(), FLG_dbPort, FLG_dbName.data());

  cout << Config::dbUrl << endl;

  table_handler.read("af");
  table_handler.create(ShortenedUrl{});
  DLOG << "db url: " << Config::dbUrl;

  WFHttpServer server(handle_request);

  if (server.start(8888) == 0) {  // start server on port 8888
    getchar();                    // press "Enter" to end.
  }

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
