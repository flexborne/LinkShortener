#pragma once

#include "crud_interface.h"
#include <string>

#include <functional>

#include <expected>

namespace dao
{

struct Error {

  const char* body;
};

class ShortenedUrlDAO : public CRUD {
 public:
  using CreationResult = std::expected<std::string, Error>;
  using CreationCallback = std::function<void(CreationResult)>;

  using ReadResult = std::expected<std::string, Error>;
 public:
  void createImpl(const char* original_url, CreationCallback callback);
};

}