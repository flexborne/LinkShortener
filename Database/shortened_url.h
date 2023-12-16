#pragma once

#include <string>
#include <chrono>

using Time = std::chrono::time_point<std::chrono::system_clock>;

struct ShortenedUrl {
  std::string shortened;
  std::string original;
  Time creation_time = std::chrono::system_clock::now();
};



