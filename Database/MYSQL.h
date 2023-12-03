#pragma once

#include "crud_interface.h"
#include <functional>
#include "../Serialization/mysql_queries.h"

#include "shortened_url.h"
#include "mysql_struct_info.h"

#include "workflow/WFMySQLConnection.h"

namespace db {
/// @brief mysql connector, that handles records of type @a Data
/// able to retrieve by primary key, add record, delete etc.
template <class Data>
class MYSQL : public CRUD {
 public:
  MYSQL() = default;

  /// @brief inserts record into db
  void create_impl(Data data);

  //using ReadResult = std::expeceted<
  void read_impl(MYSQLInfo<Data>::PrimaryKey key);

  /// @brief read only column
  void read_impl(MYSQLInfo<Data>::PrimaryKey key, MYSQLInfo<Data>::Column column);

  /// @brief custom query execution for specific situations
  void exec_custom_query(const std::string& query, mysql_callback_t callback);

  /// @brief init connection
  int init(const std::string& url);

  /// @brief creates required table in database for storing records
  /// @note blocking because table should always exist in order to store data
  void create_required_table();

 private:
  WFMySQLConnection conn{1};
};
}  // namespace db

extern template class db::MYSQL<ShortenedUrl>;
