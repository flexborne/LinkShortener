cmake_minimum_required(VERSION 3.22)
project(RequestHandlers)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(RequestHandlers INTERFACE)
target_sources(RequestHandlers INTERFACE request_handler.cpp dao.cpp)

target_include_directories(RequestHandlers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
