#pragma once

#include <cstring>  // For std::strcmp
#include <unordered_map>

#include <fmt/format.h>
#include "co/log.h"
#include "workflow/HttpMessage.h"

namespace request {

namespace status_codes {
constexpr inline char const* UNIMPLEMENTED = "501";
constexpr inline char const* OK = "200";
constexpr inline char const* NOT_FOUND = "404";
}  // namespace status_codes

namespace default_messages {
constexpr inline char const* UNSUPPORTED_URI = "Unsupported uri";
constexpr inline char const* UNSUPPORTED_METHOD = "Unsupported method";
constexpr inline char const* NOT_FOUND = "Resource not found";
}  // namespace default_messages

void reply_unsupported_method(protocol::HttpResponse* resp);

/// @brief simple base class with preprocessing job
/// calls requested method if exists, otherwise puts error into @a resp
struct RequestHandlerBase {
  void handle_request(this const auto& self, protocol::HttpRequest* req,
                      protocol::HttpResponse* resp) {
// Macro to check and call a member function if it exists
#define HANDLE_IF_EXISTS_OR_ERROR(handler_method)               \
  if constexpr (requires { self.handler_method(req, resp); }) { \
    self.handler_method(req, resp);                             \
    return;                                                     \
  } else {                                                      \
    reply_unsupported_method(resp);                             \
    return;                                                     \
  }

    const char* method = req->get_method();

    if (std::strcmp(method, "GET") == 0) {
      HANDLE_IF_EXISTS_OR_ERROR(handle_get);
    } else if (std::strcmp(method, "POST") == 0) {
      HANDLE_IF_EXISTS_OR_ERROR(handle_post);
    } else if (std::strcmp(method, "PUT") == 0) {
      HANDLE_IF_EXISTS_OR_ERROR(handle_put);
    } else if (std::strcmp(method, "DELETE") == 0) {
      HANDLE_IF_EXISTS_OR_ERROR(handle_delete);
    }

    reply_unsupported_method(resp);
#undef HANDLE_IF_EXISTS_OR_ERROR
  }
};

struct ShortenURL : RequestHandlerBase {
  void handle_get(protocol::HttpRequest* req,
                  protocol::HttpResponse* resp) const;

  void handle_post(protocol::HttpRequest* req,
                  protocol::HttpResponse* resp) const;
};

using handle_func = void (*)(protocol::HttpRequest* req,
                             protocol::HttpResponse* resp);

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
      route, +[](protocol::HttpRequest* req, protocol::HttpResponse* resp) {
        INST.handle_request(req, resp);
      });
  return 1;
}

}  // namespace request
