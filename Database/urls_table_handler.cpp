#include "urls_table_handler.h"

#include "defer.h"

#include "config.h"
#include "result_callback.h"
#include "shortened_url.h"

#include "co/log.h"

#include "workflow/MySQLResult.h"
#include "workflow/WFFacilities.h"
#include "workflow/WFTask.h"
#include "workflow/WFTaskFactory.h"

#include <format>

#define RETRY_MAX 1

namespace {
/// @brief logs and returns error considering the state of @a task
Error get_error_if_exists(WFMySQLTask* task) {
  if (task->get_state() != WFT_STATE_SUCCESS) {
    const char* error =
        WFGlobal::get_error_string(task->get_state(), task->get_error());
    ELOG << "[MYSQL] Task failed with error: " << error;
    return Error{error, ErrorCode::MySQLError};
  }
  const auto* resp = task->get_resp();
  if (resp->is_error_packet()) {
    ELOG << "[MYSQL] Task failed with error code: " << resp->get_error_code()
         << " , error message: ",
        resp->get_error_msg();
    if (resp->get_error_code() == 1062) {
      return Error{resp->get_error_msg(), ErrorCode::DuplicateEntry};
    }
    return Error{resp->get_error_msg(), ErrorCode::MySQLError};
  }
  return {};
};

using enum UrlsTableColumn;
using namespace protocol;
}  // namespace

void db::UrlsTableHandler::create_impl(ShortenedUrl url,
                                       ErrorCallback callback) const {
  auto query = std::format(
      "INSERT INTO {} ({}, {}, {}) "
      "VALUES ('{}', '{}', '{}');",
      UrlsTableInfo::TABLE_NAME, UrlsTableInfo::column_name(Shortened),
      UrlsTableInfo::column_name(Original),
      UrlsTableInfo::column_name(CreationTime), url.shortened,
      std::move(url.original), url.creation_time);

  exec_custom_query(query,
                    [res_callback = std::move(callback),
                     shortened = std::move(url.shortened)](WFMySQLTask* task) {
                      res_callback(get_error_if_exists(task));
                    });
}

void db::UrlsTableHandler::read_impl(
    UrlsTableInfo::PrimaryKey key,
    ResultCallback<ShortenedUrl> callback) const {
  auto query = std::format(
      "SELECT * FROM {} WHERE {} = '{}';", UrlsTableInfo::TABLE_NAME,
      UrlsTableInfo::column_name(Shortened), std::move(key));

  exec_custom_query(
      query, [res_callback = std::move(callback)](WFMySQLTask* task) {
        if (auto error = get_error_if_exists(task); error) {
          res_callback(std::unexpected(std::move(error)));
          return;
        }
        MySQLResultCursor cursor(task->get_resp());

        std::vector<MySQLCell> cells;
        cursor.fetch_row(cells);

        if (cells.empty()) {
          res_callback(std::unexpected(Error{{}, ErrorCode::ResourceNotFound}));
          return;
        }
        if (cells.size() != 3) [[unlikely]] {
          res_callback(std::unexpected(
              Error{{"Row size does not match size of required data type"},
                    ErrorCode::InternalError}));
          return;
        }

        std::tm tm;
        strptime(cells[2].as_datetime().c_str(), "%a %b %d %Y %H:%M:%S", &tm);

        res_callback(ShortenedUrl{
            cells[0].as_string(), cells[1].as_string(),
            std::chrono::system_clock::from_time_t(std::mktime(&tm))});
      });
}

void db::UrlsTableHandler::read_impl(UrlsTableInfo::PrimaryKey key,
                                     UrlsTableColumn column,
                                     ResultCallback<MySQLCell> callback) const {
  auto query =
      std::format("SELECT {} FROM {} WHERE {} = '{}';",
                  UrlsTableInfo::column_name(column), UrlsTableInfo::TABLE_NAME,
                  UrlsTableInfo::column_name(Shortened), std::move(key));

  exec_custom_query(
      query, [res_callback = std::move(callback)](WFMySQLTask* task) {
        if (auto error = get_error_if_exists(task); error) {
          res_callback(std::unexpected(std::move(error)));
          return;
        }
        MySQLResultCursor cursor(task->get_resp());

        std::vector<MySQLCell> cells;
        cursor.fetch_row(cells);

        if (cells.empty()) {
          res_callback(std::unexpected(Error{{}, ErrorCode::ResourceNotFound}));
          return;
        }

        res_callback(std::move(cells.front()));
      });
}

void db::UrlsTableHandler::exec_custom_query(const std::string& query,
                                             mysql_callback_t callback) const {
  WFMySQLTask* task = WFTaskFactory::create_mysql_task(Config::dbUrl, RETRY_MAX,
                                                       std::move(callback));

  task->get_req()->set_query(query);
  start_task(task);
}

void db::UrlsTableHandler::start_task(WFMySQLTask* task) const {
  if (series) [[likely]] {
    series->push_back(task);
  } else {
    task->start();
  }
}

Error db::UrlsTableHandler::create_required_table() {
  WFFacilities::WaitGroup wait_group(1);

  static const auto query =
      std::format(R"(CREATE TABLE IF NOT EXISTS {} (
            {} VARCHAR(20) PRIMARY KEY,
            {} VARCHAR(255) NOT NULL,
            {} DATETIME);)",
                  UrlsTableInfo::TABLE_NAME,
                  UrlsTableInfo::column_name(UrlsTableColumn::Shortened),
                  UrlsTableInfo::column_name(UrlsTableColumn::Original),
                  UrlsTableInfo::column_name(UrlsTableColumn::CreationTime));

  Error res;

  WFMySQLTask* task = WFTaskFactory::create_mysql_task(
      Config::dbUrl, RETRY_MAX, [&wait_group, &res](WFMySQLTask* task) {
        defer([&wait_group]() { wait_group.done(); });
        if (auto error = get_error_if_exists(task); error) {
          res = std::move(error);
          return;
        }
        return;
      });

  task->get_req()->set_query(query);
  task->start();

  wait_group.wait();

  return res;
}
