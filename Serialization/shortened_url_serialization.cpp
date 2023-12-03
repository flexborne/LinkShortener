#include "mysql_serialization.h"

#include "shortened_url.h"
#include "mysql_struct_info.h"

#include <format>
#include <string>

namespace {
  using Info = db::MYSQLInfo<ShortenedUrl>;
}
template <>
std::string serialization::mysql::insert_record_query<ShortenedUrl>(
    ShortenedUrl shortened_url) {
  using enum UrlsTableColumn;
  return std::format(
      "INSERT INTO {} ({}, {}, {}) "
      "VALUES ('{}', '{}', '{}');",
      Info::TABLE_NAME, Info::column_name(Shortened), Info::column_name(Original),
      Info::column_name(CreationTime),

      std::move(shortened_url.shortened), std::move(shortened_url.original),
      shortened_url.creation_time);
}

template <>
std::string serialization::mysql::get_record_by_primary_key_query<ShortenedUrl>(
    Info::PrimaryKey key) {
  using enum UrlsTableColumn;
  return std::format("SELECT * FROM {} WHERE {} = '{}';",
                     Info::TABLE_NAME, Info::column_name(Shortened),
                     std::move(key));
}