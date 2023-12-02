#include "request_handler.h"

namespace request {

void reply_unsupported_method(protocol::HttpResponse* resp) {
  resp->append_output_body_nocopy(default_messages::UNSUPPORTED_METHOD);
  resp->set_status_code(status_codes::UNIMPLEMENTED);
}

handle_func Router::get_handler(const char* request_uri) {
  if (auto find_res = map.find(request_uri); find_res != map.end()) {
    return find_res->second;
  }
  return nullptr;
}

void Router::add_handler(const char* route, handle_func handler) {
  map[route] = handler;
}

void ShortenURL::handle_get(protocol::HttpRequest* req,
                            protocol::HttpResponse* resp) const {
  resp->append_output_body("fuckYou)");
}

auto _ = {register_request_handler<ShortenURL>("/")};

}  // namespace request
