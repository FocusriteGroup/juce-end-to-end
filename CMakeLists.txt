cmake_minimum_required (VERSION 3.20)

set (AMPIFY_E2E_MAKE_TESTS OFF CACHE BOOL "Build example app")
set (AMPIFY_E2E_FETCH_JUCE OFF CACHE BOOL "Download JUCE")

if (AMPIFY_E2E_FETCH_JUCE)
  include (FetchContent)
  fetchcontent_declare (
    juce GIT_REPOSITORY https://github.com/juce-framework/JUCE GIT_TAG 6.1.2)

  fetchcontent_makeavailable (juce)
endif ()

project (ampify-e2e-testing)

add_subdirectory (app)

if (AMPIFY_E2E_MAKE_TESTS)
  add_subdirectory (example/app)
endif ()
