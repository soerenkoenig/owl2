cmake_minimum_required(VERSION 3.10)

project(owl VERSION 0.0.1 LANGUAGES CXX )

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/")

if (NOT WIN32 AND NOT CMAKE_BUILD_TYPE)
    message(STATUS "Current compiler: ${CMAKE_C_COMPILER}")
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror  -Wno-gnu-anonymous-struct -Wno-nested-anon-types")
endif()

add_subdirectory(owl)
add_subdirectory(tests)



