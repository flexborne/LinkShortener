#pragma once

#include <string>

#include <chrono>

using Time = std::chrono::time_point<std::chrono::system_clock>;

struct ShortenedURL {
  std::string shortenedURL;
  std::string originalURL;
  Time creationTime;
};