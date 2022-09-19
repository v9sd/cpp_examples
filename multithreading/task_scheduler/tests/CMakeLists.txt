cmake_minimum_required(VERSION 2.8)

set(TESTED_LIB "task_scheduler")

project("${TESTED_LIB}_tests")

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SRC_TESTS_FILES ${SRC_DIR}/task_scheduler.cpp)

add_executable(${PROJECT_NAME} ${SRC_DIR}/main.cpp  ${SRC_TESTS_FILES})

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../" "${CMAKE_CURRENT_BINARY_DIR}/${TESTED_LIB}/")

get_target_property("${TESTED_LIB}_INTERFACE_INCLUDE_DIRECTORY" "${TESTED_LIB}" INTERFACE_INCLUDE_DIRECTORIES)

#message(FATAL_ERROR "${${TESTED_LIB}_INTERFACE_INCLUDE_DIRECTORY}")

target_include_directories(${PROJECT_NAME} PRIVATE "${${TESTED_LIB}_INTERFACE_INCLUDE_DIRECTORY}")

target_link_libraries(${PROJECT_NAME}
                      ${TESTED_LIB}
                      gtest
                      pthread)
