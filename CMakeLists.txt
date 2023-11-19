cmake_minimum_required(VERSION 3.22)
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

# Find Boost
find_package(Boost REQUIRED COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(LinkShortener PRIVATE ${Boost_LIBRARIES})

# Find OpenSSL
find_package(OpenSSL REQUIRED)
#include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(LinkShortener PRIVATE ${OPENSSL_LIBRARIES})

# spdlog
find_package(spdlog REQUIRED)
target_link_libraries(LinkShortener PRIVATE spdlog::spdlog)

# Find the MongoDB C++ driver libraries and headers
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

# Add the include directories for the driver
target_link_libraries(LinkShortener PRIVATE mongo::mongocxx_shared mongo::bsoncxx_shared)

# fmt
find_package(fmt REQUIRED)

target_link_libraries(LinkShortener PRIVATE fmt::fmt)

find_package(workflow REQUIRED CONFIG HINTS ..)
target_link_directories(${PROJECT_NAME} PRIVATE ${WORKFLOW_LIB_DIR})