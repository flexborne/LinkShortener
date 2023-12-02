cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Serialization INTERFACE)
target_sources(Serialization INTERFACE db_record_bson.cpp)

target_include_directories(Serialization INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
