#include "MYSQL.h"

#include <fmt/core.h>

#include "../Serialization/bson.h"

#include <fmt/format.h>
#include "co/log.h"
#include "co/cout.h"
#include "workflow/MySQLResult.h"
#include "db_record.h"

void db::MYSQL::read() {

  auto callback = [](WFMySQLTask* task)
  {
    cout << "HI from callback2" << endl;
    if (task->get_state() != WFT_STATE_SUCCESS)
    {
      ELOG << "[MYSQL] Task failed with error: " << WFGlobal::get_error_string(task->get_state(), task->get_error());
      return;
    }
    cout << "EVERYTHING IS FINE";
    auto* resp = task->get_resp();
    LOG << resp->get_info() << "\n" << resp->get_affected_rows();
    cout << "VERY FINE";
    LOG << "KEK";
    //cout << task->get_resp()->

  };

  auto* task = conn.create_query_task(URLS_TABLE_CREATION_QUERY, callback);
  task->start();
}

int db::MYSQL::init(const std::string& url) {
  int res = conn.init(url);
  if (res == 0) {
    DLOG << "[MYSQL::init] successful";
    return res;
  }

  DLOG << "[MYSQL::init] error: " << res;
  return res;
}

void db::MYSQL::create(int a) {
    protocol::MySQLCell cell;
    //conn.init("mysql://root:root@localhost:3306/db?\n");
    std::string query = "SHOW TABLES;";
    auto callback = [](WFMySQLTask* task)
    {
      cout << "HI from callback2" << endl;
      if (task->get_state() != WFT_STATE_SUCCESS)
      {
        ELOG << "[MYSQL] Task failed with error: " << WFGlobal::get_error_string(task->get_state(), task->get_error());
        return;
      }
      cout << "EVERYTHING IS FINE";
      auto* resp = task->get_resp();
      LOG << resp->get_info() << "\n" << resp->get_affected_rows();
      cout << "VERY FINE";
      LOG << "KEK";
      //std::terminate();
    };

    WFMySQLTask *task = WFTaskFactory::create_mysql_task("mysql://root:root@localhost:3306/db?", 1, callback);
    task->get_req()->set_query("SHOW TABLES;");
   // WFMySQLTask* task = conn.create_query_task(query, callback);
    task->start();
}
