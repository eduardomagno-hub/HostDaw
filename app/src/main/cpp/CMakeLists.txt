cmake_minimum_required(VERSION 3.22.1)
project(hostengine)

find_package(oboe REQUIRED CONFIG)

add_library(hostengine SHARED engine.cpp)
target_link_libraries(hostengine oboe::oboe log)
