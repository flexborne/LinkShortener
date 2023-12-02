cmake_minimum_required(VERSION 3.22)
project(LinkShortener)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(RequestHandlers)
add_subdirectory(Database)
add_subdirectory(Serialization)
add_subdirectory(Utils)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE RequestHandlers)
target_link_libraries(${PROJECT_NAME} PRIVATE Database)
target_link_libraries(${PROJECT_NAME} PRIVATE Serialization)
target_link_libraries(${PROJECT_NAME} PRIVATE Utils)


list(APPEND CMAKE_PREFIX_PATH  "/home/sashka/Projects/coost/build/src")
#set(CMAKE_FIND_DEBUG_MODE ON)
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

#set(coost_DIR "/home/sashka/Projects/coost/build/src")
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

# Find Coost
find_package(coost REQUIRED CONFIG)
target_link_libraries(LinkShortener PRIVATE coost::co)

# Find OpenSSL
find_package(OpenSSL REQUIRED)
#include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(LinkShortener PRIVATE ${OPENSSL_LIBRARIES})

# Find the MongoDB C++ driver libraries and headers
find_package(mongocxx REQUIRED)
find_package(bsoncxx REQUIRED)

# Add the include directories for the driver
target_link_libraries(LinkShortener PRIVATE mongo::mongocxx_shared mongo::bsoncxx_shared)

# fmt
find_package(fmt REQUIRED)
target_link_libraries(LinkShortener PRIVATE fmt::fmt)

# Workflow
find_library(LIBRT rt)
find_package(OpenSSL REQUIRED)

find_package(workflow REQUIRED CONFIG HINTS ..)

set(WORKFLOW_LIB workflow pthread OpenSSL::SSL OpenSSL::Crypto ${LIBRT})

target_include_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_INCLUDE_DIR} ${WORKFLOW_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PRIVATE ${WORKFLOW_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${WORKFLOW_LIB})

