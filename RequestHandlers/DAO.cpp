#include "DAO.h"

auto dao::ShortenedUrlDAO::createImpl(const char* originalURL) -> std::expected<std::string, Error> {
  return std::unexpected<Error>{"1"};
}
