cmake_minimum_required(VERSION 3.24)
project(LinkShortener)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(RequestHandlers)

add_executable(LinkShortener main.cpp)
target_link_libraries(LinkShortener PRIVATE RequestHandlers)

# Find Poco
find_package(Poco REQUIRED COMPONENTS Foundation Net JSON)
include_directories(${Poco_INCLUDE_DIRS})
target_link_libraries(LinkShortener PRIVATE ${Poco_LIBRARIES})

# Find Boost
find_package(Boost REQUIRED COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(LinkShortener PRIVATE ${Boost_LIBRARIES})

# Find OpenSSL
set(OPENSSL_ROOT_DIR "/System/Volumes/Data/opt/homebrew/Cellar/openssl@3/3.0.7")
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(LinkShortener PRIVATE ${OPENSSL_LIBRARIES})

# spdlog
find_package(spdlog REQUIRED)
target_link_libraries(LinkShortener PRIVATE spdlog::spdlog)
