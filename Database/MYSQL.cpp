#include "MYSQL.h"

#include <fmt/core.h>

#include "../Serialization/mysql_queries.h"

#include <fmt/format.h>
#include "co/cout.h"
#include "co/log.h"
#include "shortened_url.h"
#include "workflow/MySQLResult.h"

#include "mysql_queries.h"
#include "mysql_struct_info.h"

template <class Data>
void db::MYSQL<Data>::read_impl(MYSQLInfo<Data>::PrimaryKey key) {

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

  auto* task = conn.create_query_task(MYSQLInfo<Data>::TABLE_CREATION_QUERY, callback);
  task->start();
}

template <class Data>
void db::MYSQL<Data>::read_impl(MYSQLInfo<Data>::PrimaryKey key, MYSQLInfo<Data>::Column column)
{
  const auto query =
      queries::mysql::get_cell_by_primary_key<Data>(std::move(key), column);

}

template <class Data>
void db::MYSQL<Data>::exec_custom_query(const std::string& query, mysql_callback_t callback)
{
  auto* task = conn.create_query_task(query, std::move(callback));
  task->start();
}

template <class Data>
int db::MYSQL<Data>::init(const std::string& url) {
  int res = conn.init(url);
  if (res == 0) {
    DLOG << "[MYSQL::init] successful";
    return res;
  }

  DLOG << "[MYSQL::init] error: " << res;
  return res;
}

template <class Data>
void db::MYSQL<Data>::create_required_table() {
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

  auto* task = conn.create_query_task(MYSQLInfo<Data>::TABLE_CREATION_QUERY, callback);
  task->start();
}

template <class Data>
void db::MYSQL<Data>::create_impl(Data data) {
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

    WFMySQLTask* task = conn.create_query_task(
        queries::mysql::insert_record(std::move(data)), callback);
    task->start();
}

template class db::MYSQL<ShortenedUrl>;