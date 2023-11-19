cmake_minimum_required(VERSION 3.22)
project(Database)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Database INTERFACE)
target_sources(Database INTERFACE mongo.cpp)

target_include_directories(Database INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
