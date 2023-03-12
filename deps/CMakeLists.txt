include(FetchContent)

message(STATUS "Fetching third-party dependencies")

set(FMT_VERSION 9.1.0)

FetchContent_Declare(
  fmt
  GIT_REPOSITORY "https://github.com/fmtlib/fmt"
  GIT_TAG a33701196adfad74917046096bf5a2aa0ab0bb50
)

FetchContent_MakeAvailable(fmt)
