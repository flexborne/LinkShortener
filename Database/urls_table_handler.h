#pragma once

#include "crud_interface.h"
#include "shortened_url.h"

#include "workflow/MySQLResult.h"
#include "workflow/WFTaskFactory.h"
#include "workflow/Workflow.h"

#include "enum_traits.h"
#include "result_callback.h"

ENUM_DEFINE(UrlsTableColumn, Shortened, Original, CreationTime)

namespace db {

struct UrlsTableInfo {
  using Column = UrlsTableColumn;
  using PrimaryKey = std::string;

  static constexpr inline std::string_view TABLE_NAME = "urls";

  static constexpr inline std::array<std::string_view,
                                     enum_size<UrlsTableColumn>()>
      TABLE_COL_NAMES{"shortened", "original", "creation_time"};

  static constexpr std::string_view column_name(UrlsTableColumn column) {
    return TABLE_COL_NAMES[static_cast<uint8_t>(column)];
  }
};

/// @brief mysql connector, that handles records of type @a Data
/// able to retrieve by primary key, add record, delete etc.
struct UrlsTableHandler : public CRUD {
 public:
  explicit UrlsTableHandler(SeriesWork* series) : series{series} {}

 public:
  /// @brief inserts record into db
  /// puts shortened url into callback if success, otherwise error
  void create_impl(ShortenedUrl url, ErrorCallback callback) const;

  /// @brief read entire row into shortened url and puts the res into @a callback
  void read_impl(UrlsTableInfo::PrimaryKey key,
                 ResultCallback<ShortenedUrl> callback) const;

  /// @brief read only column
  /// puts cell into @a callback or error
  void read_impl(UrlsTableInfo::PrimaryKey key, UrlsTableColumn column,
                 ResultCallback<protocol::MySQLCell> callback) const;

  /// @brief creates required table in database for storing records
  /// @note blocking because table should always exist in order to store data
  Error create_required_table();

 private:
  /// @brief starts task in series or independently
  void start_task(WFMySQLTask* task) const;

  /// @brief custom query execution
  void exec_custom_query(const std::string& query,
                         mysql_callback_t callback) const;

 private:
  SeriesWork* series;
};
}  // namespace db