#include "request_handler.h"
#include "urls_table_handler.h"

#include "workflow/HttpMessage.h"
#include "workflow/HttpUtil.h"
#include "workflow/WFHttpServer.h"
#include "workflow/WFTaskFactory.h"

#include "co/cout.h"
#include "co/flag.h"
#include "co/log.h"

#include "co/god.h"

#include "config.h"

#include <chrono>
#include <format>

void handle_request(WFHttpTask* task) {
  protocol::HttpRequest* req = task->get_req();
  protocol::HttpResponse* resp = task->get_resp();
  resp->set_http_version("HTTP/1.1");

  const char* request_uri = req->get_request_uri();

  request::handle_func handler = request::Router::get_handler(request_uri);

  if (handler == nullptr) [[unlikely]] {
    resp->append_output_body_nocopy(request::default_messages::UNSUPPORTED_URI);
    protocol::HttpUtil::set_response_status(resp, HttpStatusNotImplemented);
    return;
  }

  handler(task);
}

DEF_uint32(db_port, 3306, "db_port");
DEF_string(db_name, "db", "db_name");
DEF_string(db_user, "root", "db_user");
DEF_string(db_pass, "root", "db_pass");
DEF_string(db_host, "localhost", "db_host");
DEF_uint32(server_port, 8081, "server_port");

int main(int argc, char** argv) try {
  god::bless_no_bugs();

  srand(std::chrono::system_clock::now().time_since_epoch().count());

  flag::parse(argc, argv);
  cout << FLG_db_name << endl;

  Config::db_url = std::format("mysql://{}:{}@{}:{}/{}", FLG_db_user.data(),
                              FLG_db_pass.data(), FLG_db_host.data(),
                              FLG_db_port, FLG_db_name.data());

  LOG << "db url: " << Config::db_url;

  // table creation does not require series here
  db::UrlsTableHandler table_handler{nullptr};
  if (auto err = table_handler.create_required_table(); err) {
    ELOG << "Could not create required table: " << err.message;
    std::exit(1);
  }

  WFHttpServer server(handle_request);

  if (auto res = server.start(FLG_server_port); res == 0) {  
    server.wait_finish();
  } else {
    ELOG << "Could not start server. Error code: " << res;
  }
  return 0;
} catch (std::exception const& e) {
  ELOG << e.what();
} catch (...) {
  ELOG << "Unknown exception";
}
