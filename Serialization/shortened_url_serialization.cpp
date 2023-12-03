#include "mysql_queries.h"

#include "mysql_struct_info.h"
#include "shortened_url.h"

#include <format>
#include <string>

namespace {
using Info = db::MYSQLInfo<ShortenedUrl>;
}
template <>
std::string queries::mysql::insert_record<ShortenedUrl>(
    ShortenedUrl shortened_url) {
  using enum UrlsTableColumn;
  return std::format(
      "INSERT INTO {} ({}, {}, {}) "
      "VALUES ('{}', '{}', '{}');",
      Info::TABLE_NAME, Info::column_name(Shortened),
      Info::column_name(Original), Info::column_name(CreationTime),

      std::move(shortened_url.shortened), std::move(shortened_url.original),
      shortened_url.creation_time);
}

template <>
std::string queries::mysql::get_record_by_primary_key<ShortenedUrl>(
    std::basic_string<char> key) {
  using enum UrlsTableColumn;
  return std::format("SELECT * FROM {} WHERE {} = '{}';", Info::TABLE_NAME,
                     Info::column_name(Shortened), std::move(key));
}

template <>
std::string queries::mysql::get_cell_by_primary_key<ShortenedUrl>(
    std::basic_string<char> key, UrlsTableColumn column) {
  using enum UrlsTableColumn;
  return std::format("SELECT {} FROM {} WHERE {} = '{}';",
                     Info::column_name(column), Info::TABLE_NAME,
                     Info::column_name(Shortened), std::move(key));
}