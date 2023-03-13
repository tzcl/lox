add_executable(tests)

set_target_properties(tests 
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_custom_command(TARGET tests POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/testdata
    ${CMAKE_BINARY_DIR}/bin/testdata)

target_sources(tests 
	PUBLIC
    main.cpp
    scanner.test.cpp
)

target_link_libraries(tests PUBLIC doctest PRIVATE lox)