#pragma once

#include "mysql_struct_info.h"

#include <string>

namespace queries::mysql {
template <class T>
std::string insert_record(T record);

template <class T>
std::string get_record_by_primary_key(
    typename db::MYSQLInfo<T>::PrimaryKey key);

template <class T>
std::string get_cell_by_primary_key(typename db::MYSQLInfo<T>::PrimaryKey key,
                                    typename db::MYSQLInfo<T>::Column column);
}  // namespace queries::mysql