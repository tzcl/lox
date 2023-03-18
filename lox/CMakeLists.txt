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
)

target_compile_options(lox
  PRIVATE
    -Weverything  
    -Wno-c++98-compat 
    -Wno-padded 
    -Wno-sign-conversion 
    -Wno-missing-field-initializers 
    -Wno-switch-enum
    -Wno-weak-vtables
)
