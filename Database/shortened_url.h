#pragma once

#include "enum_traits.h"
#include "mysql_struct_info.h"

#include <string>
#include <chrono>

#include <string_view>
#include <format>

using Time = std::chrono::time_point<std::chrono::system_clock>;

struct ShortenedUrl {
  std::string shortened;
  std::string original;
  Time creation_time;
};

ENUM_DEFINE(UrlsTableColumn, Shortened, Original, CreationTime)

template <>
struct db::MYSQLInfo<ShortenedUrl> {
  using Column = UrlsTableColumn;
  using PrimaryKey = std::string;

  static constexpr inline std::string_view TABLE_NAME = "urls";

  static constexpr inline std::array<std::string_view,
                                     enum_size<UrlsTableColumn>()>
      TABLE_NAMES{"shortened", "original", "creation_time"};

  static constexpr std::string_view column_name(Column column) {
    return TABLE_NAMES[static_cast<uint8_t>(column)];
  }

  static const inline auto TABLE_CREATION_QUERY =
      std::format(R"(CREATE TABLE IF NOT EXISTS {} (
            {} VARCHAR(20) PRIMARY KEY,
            {} VARCHAR(255) NOT NULL,
            {} DATETIME);)",
                  TABLE_NAME, column_name(UrlsTableColumn::Shortened),
                  column_name(UrlsTableColumn::Original),
                  column_name(UrlsTableColumn::CreationTime));
};

