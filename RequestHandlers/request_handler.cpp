#include "request_handler.h"

#include "dao.h"

#include "co/json.h"
#include "workflow/HttpMessage.h"
#include "workflow/HttpUtil.h"

namespace {
using namespace protocol;

void handle_error(HttpResponse* resp, Error&& error) {
  resp->append_output_body(
      co::Json{{request::tags::ERROR, std::move(error.message)}}.str().c_str());
}

void reply_ok(HttpResponse* resp, co::Json reply_body) {
  resp->append_output_body(reply_body.str().c_str());
  HttpUtil::set_response_status(resp, HttpStatusOK);
}

}  // namespace
namespace request {

void reply_unsupported_method(protocol::HttpResponse* resp) {
  resp->append_output_body_nocopy(default_messages::UNSUPPORTED_METHOD);
  HttpUtil::set_response_status(resp, HttpStatusNotImplemented);
}
void reply_invalid_body(protocol::HttpResponse* resp) {
  resp->append_output_body_nocopy(default_messages::INVALID_BODY);
  HttpUtil::set_response_status(resp, HttpStatusBadRequest);
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

void ShortenURL::handle_get(WFHttpTask* task) const {
  /// @todo form parser instead of body for get
  auto body = HttpUtil::decode_chunked_body(task->get_req());

  auto* resp = task->get_resp();

  co::Json parsed;
  if (!parsed.parse_from(body) || !parsed.has_member(tags::SHORT)) {
    reply_invalid_body(resp);
    return;
  }

  auto short_url = parsed.get(tags::SHORT).as_c_str();

  dao::ShortenedUrlDAO dao{series_of(task)};

  dao.read(std::string{short_url}, [resp](dao::ShortenedUrlDAO::Result result) {
    if (!result.has_value()) {
      handle_error(resp, std::move(result.error()));
      return;
    }
    reply_ok(resp, co::Json{{tags::ORIGINAL, std::move(*result)}});
  });
}

void ShortenURL::handle_post(WFHttpTask* task) const {
  auto body = HttpUtil::decode_chunked_body(task->get_req());

  auto* resp = task->get_resp();

  co::Json parsed;
  if (!parsed.parse_from(body) || !parsed.has_member(tags::ORIGINAL)) {
    reply_invalid_body(resp);
    return;
  }

  auto original_url = parsed.get(tags::ORIGINAL).as_c_str();

  dao::ShortenedUrlDAO dao{series_of(task)};

  dao.create(std::string{original_url},
             [resp](dao::ShortenedUrlDAO::Result result) {
               if (!result.has_value()) {
                 handle_error(resp, std::move(result.error()));
                 return;
               }
               reply_ok(resp, co::Json{{tags::SHORT, std::move(*result)}});
             });
}

auto _ = {register_request_handler<ShortenURL>("/")};

}  // namespace request
