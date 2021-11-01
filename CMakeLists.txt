cmake_minimum_required (VERSION 3.20)

option (AMPIFY_E2E_MAKE_TESTS "Build example app")
option (AMPIFY_E2E_FETCH_JUCE "Download JUCE")

set (CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set (CMAKE_DEBUG_POSTFIX d)

if (AMPIFY_E2E_FETCH_JUCE)
  include (cmake/fetch-juce.cmake)
endif ()

include (cmake/replace-default-vs-variables.cmake)
include (cmake/set_common_target_properties.cmake)

project (ampify-e2e-testing)

include (CTest)
enable_testing ()

add_subdirectory (source/cpp)

if (AMPIFY_E2E_MAKE_TESTS)
  add_subdirectory (example/app)
endif ()
