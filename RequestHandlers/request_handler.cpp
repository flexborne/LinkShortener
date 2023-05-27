#include "request_handler.h"

#include <spdlog/spdlog.h>
#include "../Utils/compile_time_utils.h"

namespace {

template <typename T, http::verb method>
concept has_handle_impl =
    requires(T t, request::Request& req, request::Response& res) {
      { T::template handle_impl<method>(req, res) } -> std::same_as<void>;
    };

template <typename T, http::verb method>
constexpr bool has_handle_impl_v = has_handle_impl<T, method>;

using request_handler_operation = void (*)(request::Request&,
                                           request::Response&);

using array_type =
    std::array<std::array<request_handler_operation, enum_size<http::verb>()>,
               enum_size<request::Target>()>;

consteval array_type generate_array() {
  array_type array{nullptr};

  utils::for_<enum_size<http::verb>()>([&](const auto i) {
    constexpr http::verb value = static_cast<http::verb>(i.value);
    if constexpr (has_handle_impl_v<request::ConcreteRequestHandler, value>) {
      array[static_cast<size_t>(request::Target::Kek)][toNum(value)] =
          &request::ConcreteRequestHandler::handle_impl<value>;
    }
  });

  return array;
}
constexpr const auto handlers = generate_array();
}  // namespace

void request::Router::route(Request& request, Response& response) {
  auto* target = handlers[toNum(request::Target::Kek)][toNum(request.method())];

  if (target == nullptr) {
    response.result(http::status::not_found);
    response.set(http::field::content_type, "text/plain");
    response.body() = "Request Not Found";
    return;
  }

  std::invoke(target, request, response);
}
