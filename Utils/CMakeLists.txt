cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Utils INTERFACE)
target_sources(Utils INTERFACE shortener.cpp)

target_include_directories(Utils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
