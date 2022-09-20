cmake_minimum_required(VERSION 2.8)

project(accurate_counter)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(SRC_FILES "${SRC_DIR}/accurate_counter.cpp")

add_library(${PROJECT_NAME} STATIC ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")




