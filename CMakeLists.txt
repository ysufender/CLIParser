cmake_minimum_required(VERSION 3.30)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(CLIParser 
    VERSION 1.0
    DESCRIPTION "Parses the CLI"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_executable(CLIParser CLIParser.cpp debug.cpp)
else()
    add_library(CLIParser STATIC CLIParser.cpp)
endif()
