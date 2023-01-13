cmake_minimum_required(VERSION 3.25)

# Make sure tooling and projects use the same version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(lox
  DESCRIPTION "An implementation of the Lox language in C++"
  LANGUAGES CXX)

add_executable(lox lox/main.cpp)

target_include_directories(lox PUBLIC ${lox_SOURCE_DIR})

find_package(fmt)
target_link_libraries(lox fmt::fmt)