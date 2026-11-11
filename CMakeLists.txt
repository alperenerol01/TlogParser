cmake_minimum_required(VERSION 3.21)

project(TlogParser VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(MAVLINK_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/third_party/c_library_v2/common")
if(NOT EXISTS "${MAVLINK_INCLUDE_DIR}/mavlink.h")
    message(FATAL_ERROR
        "MAVLink headers not found in ${MAVLINK_INCLUDE_DIR}.\n"
        "Run: git submodule update --init")
endif()

add_subdirectory(src/core)
add_subdirectory(src/app)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
