cmake_minimum_required(VERSION 3.26)
project(LinkShortener)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(RequestHandlers)
add_subdirectory(Database)
add_subdirectory(Serialization)
add_subdirectory(Utils)

add_executable(LinkShortener main.cpp)

target_link_libraries(LinkShortener PRIVATE RequestHandlers)
target_link_libraries(LinkShortener PRIVATE Database)
target_link_libraries(LinkShortener PRIVATE Serialization)
target_link_libraries(LinkShortener PRIVATE Utils)

# Find Poco
find_package(Poco REQUIRED COMPONENTS Foundation Net JSON)
include_directories(${Poco_INCLUDE_DIRS})
target_link_libraries(LinkShortener PRIVATE ${Poco_LIBRARIES})

# Find Boost
find_package(Boost REQUIRED COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(LinkShortener PRIVATE ${Boost_LIBRARIES})

# Find OpenSSL
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(LinkShortener PRIVATE ${OPENSSL_LIBRARIES})

# spdlog
find_package(spdlog REQUIRED)
target_link_libraries(LinkShortener PRIVATE spdlog::spdlog)

# Find the MongoDB C++ driver libraries and headers
find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

# Add the include directories for the driver
include_directories(${LIBMONGOCXX_INCLUDE_DIRS} ${LIBBSONCXX_INCLUDE_DIRS})
target_link_libraries(LinkShortener PRIVATE ${LIBMONGOCXX_LIBRARIES} ${LIBBSONCXX_LIBRARIES})

# fmt
find_package(fmt REQUIRED)

target_link_libraries(LinkShortener PRIVATE fmt::fmt)

