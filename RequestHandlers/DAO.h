#pragma once

#include "crud_interface.h"
#include <string>

#include <expected>

namespace dao
{

struct Error {

  const char* body;
};

class ShortenedUrlDAO : public CRUD {
 public:
  auto createImpl(const char* originalURL) -> std::expected<std::string, Error>;
};

}