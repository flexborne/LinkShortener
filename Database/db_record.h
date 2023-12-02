#pragma once

#include <string>

#include <chrono>

using Time = std::chrono::time_point<std::chrono::system_clock>;

struct ShortenedURL {
  std::string shortenedURL;
  std::string originalURL;
  Time creationTime;
};

const inline std::string URLS_TABLE_CREATION_QUERY =
                          R"(CREATE TABLE IF NOT EXISTS urls (
                            shortened VARCHAR(20) PRIMARY KEY,
                            original VARCHAR(255) NOT NULL,
                            creation_time DATETIME
                          );)";