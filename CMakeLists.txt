cmake_minimum_required(VERSION 3.28)
project(rpc)

set(CMAKE_CXX_STANDARD 23)

find_package(JNI REQUIRED)

file(GLOB_RECURSE SOURCES src/**)

#add_library(rpc SHARED ${SOURCES})
add_executable(rpc ${SOURCES})

target_include_directories(rpc PRIVATE ${JNI_INCLUDE_DIRS})

target_link_libraries(rpc PRIVATE ${JNI_LIBRARIES})