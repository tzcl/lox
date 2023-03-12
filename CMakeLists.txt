cmake_minimum_required(VERSION 3.25)

# Make sure tooling and projects use the same version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(lox
  DESCRIPTION "An implementation of the Lox language in C++"
  LANGUAGES CXX
)

add_compile_options("-Weverything"  "-Wno-c++98-compat" "-Wno-padded" 
  "-Wno-sign-conversion" "-Wno-missing-field-initializers" "-Wno-switch-enum")

add_subdirectory(deps SYSTEM)
add_subdirectory(lox)
add_subdirectory(repl)
