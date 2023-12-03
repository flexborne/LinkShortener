#pragma once

#include "mysql_struct_info.h"

#include <string>

namespace serialization::mysql
{
  template <class T>
  std::string insert_record_query(T record);

  template <class T>
  std::string get_record_by_primary_key_query(typename db::MYSQLInfo<T>::PrimaryKey key);
}