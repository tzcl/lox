add_library(lox)

target_include_directories(lox
  PUBLIC 
    ${lox_SOURCE_DIR}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(lox PUBLIC fmt::fmt)

add_subdirectory(parser)
add_subdirectory(interpreter)

target_sources(lox PUBLIC errors.cpp)

target_compile_options(lox
  PRIVATE
    -Weverything  
    -Wno-c++98-compat 
    -Wno-padded 
    -Wno-sign-conversion 
    -Wno-missing-field-initializers 
    -Wno-switch-enum
)
