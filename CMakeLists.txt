cmake_minimum_required(VERSION 3.18)

project(wadjet LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(WADJET_STATIC "Enable static instead of shared mode." OFF)
option(WADJET_BUILD_TESTS "Enable automated tests." ON)
option(WADJET_BUILD_EXAMPLES "Build example applications." OFF)

set(WADJET_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(src)
if(WADJET_BUILD_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
if(WADJET_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

install(FILES LICENSE DESTINATION .)