cmake_minimum_required(VERSION 3.18)

project(graphics_of_counters
       VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        ./src/main.cpp
        ./src/mainwindow.cpp
        ./src/mainwindow.h
        ./src/mainwindow.ui
        ./src/points_generator.h
        ./src/points_generator.cpp)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

#target_compile_definitions(${PROJECT_NAME} PRIVATE QT_NO_PRINTER=YES)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../accurate_counter" "${CMAKE_CURRENT_BINARY_DIR}/accurate_counter/")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../non_accurate_counter" "${CMAKE_CURRENT_BINARY_DIR}/non_accurate_counter/")

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets qcustomplot accurate_counter non_accurate_counter)
