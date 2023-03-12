include(FetchContent)

message(STATUS "Fetching third-party dependencies")

set(FMT_VERSION 9.1.0)

FetchContent_Declare(
  fmt
  SYSTEM
  GIT_REPOSITORY "https://github.com/fmtlib/fmt"
  GIT_TAG a33701196adfad74917046096bf5a2aa0ab0bb50
)

FetchContent_Declare(
  doctest
  SYSTEM
  GIT_REPOSITORY "https://github.com/doctest/doctest"
  GIT_TAG 8fdfd113dcb4ad1a31705ff8ddb13a21a505bad8
)
include_directories(${DOCTEST_INCLUDE_DIR})

FetchContent_MakeAvailable(fmt doctest)
