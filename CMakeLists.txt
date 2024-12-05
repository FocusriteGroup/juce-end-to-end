cmake_minimum_required (VERSION 3.20)

option (FOCUSRITE_E2E_MAKE_TESTS "Build example app")
option (FOCUSRITE_E2E_FETCH_JUCE "Download JUCE")

set (CMAKE_DEBUG_POSTFIX d)
set (CXX_STANDARD_REQUIRED 17)

if (FOCUSRITE_E2E_FETCH_JUCE)
  include (cmake/fetch-juce.cmake)
endif ()

include (cmake/set_common_target_properties.cmake)

project (focusrite-e2e-testing)

include (CTest)
enable_testing ()

add_subdirectory (source/cpp)

if (FOCUSRITE_E2E_MAKE_TESTS)
  add_subdirectory (example/app)
endif ()
