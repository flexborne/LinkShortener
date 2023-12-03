#include "urls_table_handler.h"

#include "co/cout.h"
#include "co/log.h"
#include "shortened_url.h"
#include "workflow/MySQLResult.h"
#include "workflow/WFTaskFactory.h"
#include "config.h"

#include <format>

#define RETRY_MAX 1

namespace {
using enum UrlsTableColumn;
}

void mysql_callback(WFMySQLTask* task) {
  if (task->get_state() != WFT_STATE_SUCCESS) {
    ELOG << "[MYSQL] Task failed with error: "
         << WFGlobal::get_error_string(task->get_state(), task->get_error());
    return;
  }
  cout << "EVERYTHING IS FINE";
  auto* resp = task->get_resp();
  LOG << resp->get_info() << "\n" << resp->get_affected_rows();
  cout << "VERY FINE";
  LOG << "KEK";
  //cout << task->get_resp()->
}

void db::UrlsTableHandler::create_impl(ShortenedUrl url) {
  auto query = std::format(
      "INSERT INTO {} ({}, {}, {}) "
      "VALUES ('{}', '{}', '{}');",
      UrlsTableInfo::TABLE_NAME, UrlsTableInfo::column_name(Shortened),
      UrlsTableInfo::column_name(Original), UrlsTableInfo::column_name(CreationTime),

      std::move(url.shortened), std::move(url.original),
      url.creation_time);
  WFMySQLTask *task = WFTaskFactory::create_mysql_task(Config::dbUrl, RETRY_MAX, mysql_callback);
  task->get_req()->set_query(query);
  task->start();
}

void db::UrlsTableHandler::read_impl(UrlsTableInfo::PrimaryKey key, ResultCallback<ShortenedUrl> callback) {
  WFMySQLTask *task = WFTaskFactory::create_mysql_task(Config::dbUrl, RETRY_MAX, mysql_callback);

  auto query = std::format("SELECT * FROM {} WHERE {} = '{}';", UrlsTableInfo::TABLE_NAME,
                           UrlsTableInfo::column_name(Shortened), std::move(key));


  task->get_req()->set_query(query);
  task->start();

}

void db::UrlsTableHandler::read_impl(UrlsTableInfo::PrimaryKey key,
                                     UrlsTableColumn column) {
  auto query = std::format("SELECT {} FROM {} WHERE {} = '{}';",
                     UrlsTableInfo::column_name(column), UrlsTableInfo::TABLE_NAME,
                     UrlsTableInfo::column_name(Shortened), std::move(key));

  WFMySQLTask *task = WFTaskFactory::create_mysql_task(Config::dbUrl, RETRY_MAX, mysql_callback);

  task->get_req()->set_query(query);
  task->start();
}

void db::UrlsTableHandler::exec_custom_query(const std::string& query,
                                             mysql_callback_t callback) {
  WFMySQLTask *task = WFTaskFactory::create_mysql_task(Config::dbUrl, RETRY_MAX, mysql_callback);
  task->get_req()->set_query(query);
  task->start();
}

void db::UrlsTableHandler::create_required_table() {
  static const auto query =
      std::format(R"(CREATE TABLE IF NOT EXISTS {} (
            {} VARCHAR(20) PRIMARY KEY,
            {} VARCHAR(255) NOT NULL,
            {} DATETIME);)",
                  UrlsTableInfo::TABLE_NAME,
                  UrlsTableInfo::column_name(UrlsTableColumn::Shortened),
                  UrlsTableInfo::column_name(UrlsTableColumn::Original),
                  UrlsTableInfo::column_name(UrlsTableColumn::CreationTime));

  WFMySQLTask *task = WFTaskFactory::create_mysql_task(Config::dbUrl, RETRY_MAX, mysql_callback);
  task->get_req()->set_query(query);

  task->start();
}
