cmake_minimum_required(VERSION 3.20)
project(ss-tcp-latency LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    message(STATUS "Using ccache ${CCACHE_FOUND}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ${CCACHE_FOUND})
endif()

set(Boost_USE_STATIC_LIBS ON)

add_subdirectory(3rdparty/fmt)
add_subdirectory(3rdparty/seastar)
add_subdirectory(src)
