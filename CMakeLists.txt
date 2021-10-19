cmake_minimum_required (VERSION 3.20)

option (AMPIFY_E2E_MAKE_TESTS "Build example app")
option (AMPIFY_E2E_FETCH_JUCE "Download JUCE")

set (CMAKE_DEBUG_POSTFIX d)

if (AMPIFY_E2E_FETCH_JUCE)
  include (cmake/fetch-juce.cmake)
endif()

include (cmake/replace-default-vs-variables.cmake)

project (ampify-e2e-testing)

add_subdirectory (app)

if (AMPIFY_E2E_MAKE_TESTS)
  add_subdirectory (example/app)
endif ()
