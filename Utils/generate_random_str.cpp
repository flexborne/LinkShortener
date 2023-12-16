#include "generate_random_str.h"

namespace {
constexpr char alphanum[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";

constexpr auto SIZE_OF_LOOKUP_TABLE = sizeof(alphanum);
constexpr size_t SIZE_OF_STR = 8;
}  // namespace

std::string utils::generate_random_string() {

  std::string res;
  res.reserve(SIZE_OF_STR);

  for (int i = 0; i < SIZE_OF_STR; ++i) {
    res += alphanum[rand() % (SIZE_OF_LOOKUP_TABLE - 1)];
  }

  return res;
}