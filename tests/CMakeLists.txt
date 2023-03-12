add_executable(tests)
set_target_properties(tests 
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_sources(tests 
	PUBLIC
    main.cpp
    scanner.test.cpp
)

target_link_libraries(tests PUBLIC doctest PRIVATE lox)