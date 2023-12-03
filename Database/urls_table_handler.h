#pragma once

#include "crud_interface.h"
#include "shortened_url.h"
#include "urls_table_handler.h"

#include "workflow/WFMySQLConnection.h"

#include <expected>

#include "enum_traits.h"


struct Error{
  const char* message;
};

template <class Data>
using Result = std::expected<Data, Error>;

template <class Data>
using ResultCallback = std::function<void(Result<Data>)>;

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
  /// @brief inserts record into db
  void create_impl(ShortenedUrl url);

  //using ReadResult = std::expeceted<
  void read_impl(UrlsTableInfo::PrimaryKey key, ResultCallback<ShortenedUrl> callback);

  /// @brief read only column
  void read_impl(UrlsTableInfo::PrimaryKey key, UrlsTableColumn column);

  /// @brief custom query execution for specific situations
  void exec_custom_query(const std::string& query, mysql_callback_t callback);

  /// @brief creates required table in database for storing records
  /// @note blocking because table should always exist in order to store data
  void create_required_table();
};
}  // namespace db