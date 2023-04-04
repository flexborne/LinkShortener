cmake_minimum_required(VERSION 3.24)
project(RequestHandlers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(RequestHandlers INTERFACE)
target_sources(RequestHandlers INTERFACE sv_RequestHandler.cpp)

target_include_directories(RequestHandlers INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
