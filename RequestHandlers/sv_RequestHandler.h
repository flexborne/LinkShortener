#include <string>
#include <unordered_map>
#include <functional>
#include <boost/beast.hpp>

#include "../Utils/utils_EnumTraits.h"


/// @brief I decided to create constexpr array with CRTPs request handlers in order to implement routing mechanism
/// This application has to be high-performance, so it is unappropriated to waste it on runtime dynamic polymorphism

namespace http = boost::beast::http;

ENUM_DEFINE_NS(request, Target, Kek)

template<>
[[maybe_unused]] constexpr uint8_t enum_size<http::verb>() {
    return static_cast<uint8_t>(http::verb::unlink) + 1;
}

namespace request {


    using Request = http::request<http::string_body>;
    using Response = http::response<http::string_body>;

    class DummyAuth {
    public:
        static bool check_auth(Request& request) { return true; }
    };


    template<class Derived, class Auth>
    class RequestHandler {
    public:
        template<http::verb method>
        static void handle(Request &request, Response &response) {
            if (!Auth::check_auth(request)) {
                return;
            }

            Derived::template handle_impl<method>(request, response);
        }

    private:
        Derived *derived() {
            return static_cast<Derived *>(this);
        }

    private:
        Auth auth;
    };


    class Router {
    public:
        static void route(Request &request, Response &response);
    };

    class ConcreteRequestHandler : public RequestHandler<ConcreteRequestHandler, DummyAuth> {
    public:
        // template magic
        template<http::verb method> requires (method == http::verb::post)
        static void handle_impl(Request& request, Response& response);

        template<>
        static void handle_impl<http::verb::post>(Request& request, Response& response) {
            response.body() = "FOUND";
        }
    };


} // namespace request

