add_executable(repl)
set_target_properties(repl
  PROPERTIES 
    OUTPUT_NAME "lox"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

target_sources(repl PRIVATE main.cpp)

target_link_libraries(repl PRIVATE lox)
