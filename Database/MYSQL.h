#pragma once

#include "crud_interface.h"

#include "../Serialization/mysql_serialization.h"

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

  void read_impl(MYSQLInfo<Data>::PrimaryKey key);

  /// @brief init connection
  int init(const std::string& url);

  /// @brief creates required table in database for storing records
  /// @note blocking because table should always exist in order to store data
  void create_required_table();

 private:
  WFMySQLConnection conn{1};
  inline static const std::string TABLE = "table";
};
}  // namespace db

extern template class db::MYSQL<ShortenedUrl>;
