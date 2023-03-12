add_executable(lox_repl)
set_target_properties(lox_repl 
  PROPERTIES 
    OUTPUT_NAME "lox"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_sources(lox_repl PRIVATE main.cpp)

target_link_libraries(lox_repl PRIVATE lox)
