cmake_minimum_required(VERSION 3.5)

project(cnn-example)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

# Set compile flags for Debug and Release modes
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -frtti")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-g -O3")

# Print the build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Debug option
option(USE_DEBUG "Enter debug mode" OFF)
if(USE_DEBUG)
    add_definitions(-DDEBUG)
    message(STATUS "Debug mode: ON")
else()
    message(STATUS "Debug mode: OFF")
endif()

# Rest of your existing configuration
SET("OpenCV_DIR" "/usr/include/opencv4")
find_package(Boost REQUIRED COMPONENTS serialization system filesystem iostreams)
find_package(OpenCV REQUIRED)
find_package(ZLIB REQUIRED)

MESSAGE(STATUS "Include dirs ${OpenCV_INCLUDE_DIRS}")
MESSAGE(STATUS "LINK LIBRARIES ${OpenCV_LIBS}")

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(DIVISIBLE_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(DIVISIBLE_INSTALL_BIN_DIR ${PROJECT_SOURCE_DIR}/bin)
set(DIVISIBLE_INSTALL_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)

set(YAML_HEADERS_DIR ${PROJECT_SOURCE_DIR}/src/boost-archive-yaml)
set(CNN_HEADERS_DIR  ${PROJECT_SOURCE_DIR}/include)

include_directories(${DIVISIBLE_INSTALL_INCLUDE_DIR})
include_directories(${DIVISION_HEADERS_DIR})

add_subdirectory(src)
add_subdirectory(test)
