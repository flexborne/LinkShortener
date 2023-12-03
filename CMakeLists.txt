cmake_minimum_required(VERSION 3.22)
project(LinkShortener)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

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


list(APPEND CMAKE_PREFIX_PATH  "/home/sashka/Projects/coost/cmake-build-release-wsl/src")

#set(CMAKE_FIND_DEBUG_MODE ON)
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

#set(coost_DIR "/home/sashka/Projects/coost/build/src")
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")

# Find Coost
find_package(coost REQUIRED CONFIG)
target_link_libraries(${PROJECT_NAME} PRIVATE coost::co)

# Find OpenSSL
find_package(OpenSSL REQUIRED)
#include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENSSL_LIBRARIES})


# Workflow
find_library(LIBRT rt)
find_package(OpenSSL REQUIRED)

#list(APPEND CMAKE_PREFIX_PATH  "/home/sashka/Projects/workflow/cmake-build-release-wsl")
#set(workflow_DIR "/home/sashka/Projects/workflow/cmake-build-release-wsl")
set(CMAKE_FIND_DEBUG_MODE ON)
find_package(workflow REQUIRED CONFIG HINTS ..)
message(STATUS "WORKFLOW_LIB_DIR is ${WORKFLOW_LIB_DIR}")

set(WORKFLOW_LIB workflow pthread OpenSSL::SSL OpenSSL::Crypto ${LIBRT})

target_include_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_INCLUDE_DIR} ${WORKFLOW_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PRIVATE ${WORKFLOW_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${WORKFLOW_LIB})

