add_library(lox)

target_include_directories(lox
  PUBLIC 
    ${lox_SOURCE_DIR}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(lox PUBLIC fmt::fmt)

target_sources(lox 
  PUBLIC 
    errors.cpp
    token/token.cpp
    scanner/scanner.cpp
    parser/parser.cpp
    interpreter/value.cpp
    interpreter/interpreter.cpp
    interpreter/environment.cpp
)

target_compile_options(lox
  PRIVATE
    -Weverything  
    -Wno-c++98-compat 
    -Wno-pre-c++20-compat-pedantic
    -Wno-padded 
    -Wno-sign-conversion 
    -Wno-missing-field-initializers 
    -Wno-switch-enum
    -Wno-weak-vtables
)
