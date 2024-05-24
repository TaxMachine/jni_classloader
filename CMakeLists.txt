cmake_minimum_required(VERSION 3.28)
project(rpc)

set(CMAKE_CXX_STANDARD 23)

if (MSVC)
    find_package(JNI REQUIRED)
    include_directories(${JNI_INCLUDE_DIRS})
else ()
    set(JVM_DIR /usr/lib/jvm/java-1.17.0-openjdk-amd64)
    include_directories(${JVM_DIR}/include ${JVM_DIR}/include/linux)
    set(JNI_LIBRARIES ${JVM_DIR}/lib/server/libjvm.so)
endif ()


file(GLOB_RECURSE SOURCES src/**)

#add_library(rpc SHARED ${SOURCES})
add_executable(rpc ${SOURCES})

target_include_directories(rpc PRIVATE ${JNI_INCLUDE_DIRS})

target_link_libraries(rpc PRIVATE ${JNI_LIBRARIES})