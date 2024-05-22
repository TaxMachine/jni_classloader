cmake_minimum_required(VERSION 3.28)
project(rpc)

set(CMAKE_CXX_STANDARD 23)
set(JVM_DIR /usr/lib/jvm/java-1.17.0-openjdk-amd64)

file(GLOB_RECURSE SOURCES src/**)
include_directories(${JVM_DIR}/include ${JVM_DIR}/include/linux)

#add_library(rpc SHARED ${SOURCES})
add_executable(rpc ${SOURCES})

target_include_directories(rpc PRIVATE ${JNI_INCLUDE_DIRS})

target_link_libraries(rpc PRIVATE ${JVM_DIR}/lib/server/libjvm.so)