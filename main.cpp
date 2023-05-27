#include <spdlog/spdlog.h>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include "RequestHandlers/request_handler.h"
#include "db_record.h"
#include "mongo.h"

namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

// The SSL context holds certificates
ssl::context create_ssl_context() {
  ssl::context ctx{ssl::context::tlsv12};
  ctx.set_default_verify_paths();
  ctx.use_certificate_chain_file("../server.crt");
  ctx.use_private_key_file("../server.key", ssl::context::pem);
  return ctx;
}

struct ServerConfig {
  constexpr static uint16_t port_num = 443;
  static inline auto const ip_address = tcp::v4();
};

// The session handles a single HTTPS request
class Session : public std::enable_shared_from_this<Session> {
 public:
  // Construct the session with a connected socket
  explicit Session(tcp::socket&& socket, ssl::context& ssl_context)
      : stream_(std::move(socket), ssl_context) {}

  // Start the session
  void start() { do_handshake(); }

 private:
  // Perform SSL handshake
  void do_handshake() {
    auto self = shared_from_this();
    stream_.async_handshake(ssl::stream_base::server,
                            [self](beast::error_code ec) {
                              if (!ec)
                                self->do_read();
                            });
  }

  // Read an HTTP request
  void do_read() {
    auto self = shared_from_this();
    beast::http::async_read(
        stream_, buffer_, request_,
        [self](beast::error_code ec, std::size_t bytes_transferred) {
          spdlog::info("request target {}", self->request_.target());
          if (!ec)
            self->do_write();
        });
  }

  // Send an HTTP response
  void do_write() {
    auto self = shared_from_this();
    response_.result(beast::http::status::ok);
    response_.version(request_.version());
    response_.set(beast::http::field::server, "Beast");
    response_.set(beast::http::field::content_type, "text/plain");
    // response_.body() = "Hello, world!";

    request::Router::route(request_, response_);
    response_.prepare_payload();

    beast::http::async_write(
        stream_, response_,
        [self](beast::error_code ec, std::size_t bytes_transferred) {
          if (ec) {
            spdlog::error(ec.what());
          }
          self->do_read();
        });
  }

  // The SSL stream
  ssl::stream<tcp::socket> stream_;

  // The buffer for the HTTP request
  beast::flat_buffer buffer_{8192};

  request::Response response_;

  // The HTTP request
  request::Request request_;
};

// Accept and handle HTTPS connections
void do_accept(tcp::acceptor& acceptor, ssl::context& ssl_context) {
  acceptor.async_accept([&](beast::error_code ec, tcp::socket&& socket) {
    if (!ec) {
      spdlog::info("Accepted new connection");

      // Create a session and start it
      std::make_shared<Session>(std::move(socket), ssl_context)->start();
    } else {
      spdlog::error(ec.what());
    }
    spdlog::info("Adding new connection");
    do_accept(acceptor, ssl_context);
  });
}

int main() try {
  auto connector = database::MongoConnector{"localhost", 27017};
  connector.create(DbRecord{.key_ = "dadada", .url_ = "URL"});
  asio::io_context ioc;
  tcp::acceptor acceptor{ioc,
                         {ServerConfig::ip_address, ServerConfig::port_num}};
  ssl::context ssl_context{ssl::context::tlsv12};
  ssl_context = create_ssl_context();

  // Start accepting HTTPS connectionsdo_accept(acceptor, ssl_context);

  // Run the I/O service

  ioc.run();
  return 0;
} catch (std::exception const& e) {
  spdlog::error(e.what());
} catch (...) {
  spdlog::error("Unknown exception");
}
