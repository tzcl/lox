cmake_minimum_required(VERSION 3.25)

# Make sure tooling and projects use the same version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use Homebrew not Xcode toolchain
if (${APPLE})
  set(CMAKE_PREFIX_PATH /opt/homebrew)
  set(CMAKE_FIND_FRAMEWORK NEVER)
  set(CMAKE_FIND_APPBUNDLE NEVER)
endif()

project(lox
  DESCRIPTION "An implementation of the Lox language in C++"
  LANGUAGES CXX)
add_compile_options("-Weverything"  "-Wno-c++98-compat" "-Wno-padded" 
  "-Wno-sign-conversion" "-Wno-missing-field-initializers")

set(SOURCES lox/main.cpp lox/token.cpp lox/scanner.cpp lox/error.cpp 
  lox/parser.cpp)

add_executable(lox ${SOURCES})

target_include_directories(lox PUBLIC ${lox_SOURCE_DIR})

find_package(fmt)
target_link_libraries(lox fmt::fmt)