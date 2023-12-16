#pragma once

#include "co/log.h"
#include "workflow/HttpMessage.h"
#include "workflow/HttpUtil.h"
#include "workflow/WFTaskFactory.h"

#include <fmt/format.h>
#include <cstring>
#include <unordered_map>

namespace request {

namespace default_messages {
constexpr inline char const* UNSUPPORTED_URI = "Unsupported uri";
constexpr inline char const* UNSUPPORTED_METHOD = "Unsupported method";
constexpr inline char const* NOT_FOUND = "Resource not found";
constexpr inline char const* INVALID_BODY = "Invalid body";
}  // namespace default_messages

namespace tags {
constexpr inline char const* SHORT = "short";
constexpr inline char const* ORIGINAL = "original";
constexpr inline char const* ERROR = "error";
}  // namespace tags

void reply_unsupported_method(protocol::HttpResponse* resp);
void reply_invalid_body(protocol::HttpResponse* resp);

/// @brief simple base class with preprocessing job
/// calls requested method if exists, otherwise puts error into @a resp
struct RequestHandlerBase {
  void handle_request(this const auto& self, WFHttpTask* task) {
// Macro to check and call a member function if it exists
#define HANDLE_IF_EXISTS_OR_ERROR(handler_method)          \
  if constexpr (requires { self.handler_method(task); }) { \
    self.handler_method(task);                             \
    return;                                                \
  } else {                                                 \
    reply_unsupported_method(task->get_resp());            \
    return;                                                \
  }

    const char* method = task->get_req()->get_method();

    if (std::strcmp(method, HttpMethodGet) == 0) {
      HANDLE_IF_EXISTS_OR_ERROR(handle_get);
    } else if (std::strcmp(method, HttpMethodPost) == 0) {
      HANDLE_IF_EXISTS_OR_ERROR(handle_post);
    } else if (std::strcmp(method, HttpMethodPut) == 0) {
      HANDLE_IF_EXISTS_OR_ERROR(handle_put);
    } else if (std::strcmp(method, HttpMethodDelete) == 0) {
      HANDLE_IF_EXISTS_OR_ERROR(handle_delete);
    }

    reply_unsupported_method(task->get_resp());
#undef HANDLE_IF_EXISTS_OR_ERROR
  }
};

struct ShortenURL : RequestHandlerBase {
  void handle_get(WFHttpTask* task) const;

  void handle_post(WFHttpTask* task) const;
};

using handle_func = void (*)(WFHttpTask* task);

struct Router {
  /// @return handler func pointer if exists, nullptr otherwise
  static handle_func get_handler(const char* request_uri);

  /// @brief adds func pointer to the routing map
  static void add_handler(const char* route, handle_func handler);

 private:
  static inline std::unordered_map<std::string, handle_func> map;
};

template <class RequestHandler>
int register_request_handler(const char* route) {
  static constexpr RequestHandler INST;
  Router::add_handler(
      route, +[](WFHttpTask* task) { INST.handle_request(task); });
  return 1;
}

}  // namespace request
