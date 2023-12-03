#include "request_handler.h"

#include "dao.h"

#include "co/cout.h"
#include "co/json.h"
#include "workflow/HttpUtil.h"

namespace request {

void reply_unsupported_method(protocol::HttpResponse* resp) {
  resp->append_output_body_nocopy(default_messages::UNSUPPORTED_METHOD);
  protocol::HttpUtil::set_response_status(resp, HttpStatusNotImplemented);
}
void reply_invalid_body(protocol::HttpResponse* resp) {
  resp->append_output_body_nocopy(default_messages::INVALID_BODY);
  protocol::HttpUtil::set_response_status(resp, HttpStatusBadRequest);
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
  auto body = protocol::HttpUtil::decode_chunked_body(req);

  co::Json parsed;
  if (!parsed.parse_from(body) || !parsed.has_member(tags::KEY)) {
    reply_invalid_body(resp);
    return;
  }

  auto key = parsed.get(tags::KEY).as_c_str();

  dao::ShortenedUrlDAO dao;
  resp->append_output_body(key);
}

void ShortenURL::handle_post(protocol::HttpRequest* req,
                             protocol::HttpResponse* resp) const {

}

auto _ = {register_request_handler<ShortenURL>("/")};

}  // namespace request
