cmake_minimum_required (VERSION 3.20)

option (FOCUSRITE_E2E_MAKE_TESTS "Build example app")
option (FOCUSRITE_E2E_FETCH_JUCE "Download JUCE")

set (CMAKE_DEBUG_POSTFIX d)

if (FOCUSRITE_E2E_FETCH_JUCE)
  include (cmake/fetch-juce.cmake)
endif ()

include (cmake/set_common_target_properties.cmake)

project (focusrite-e2e-testing)

include (CTest)
enable_testing ()

add_subdirectory (source/cpp)

if (FOCUSRITE_E2E_MAKE_TESTS)
  include(CTest)

  if (WIN32)
    set (NPM_COMMAND npm.cmd)
    set (NPX_COMMAND npx.cmd)
  else()
    set (NPM_COMMAND npm)
    set (NPX_COMMAND npx)
  endif()

  add_test(
    NAME npm-install
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${NPM_COMMAND} install
  )
  set_tests_properties(npm-install
  PROPERTIES
    FIXURES_SETUP NPM_INSTALL
  )

  add_subdirectory (example/app)

  add_test(
    NAME test-ts-lib
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${NPX_COMMAND} jest "${CMAKE_SOURCE_DIR}/tests"
  )
  set_tests_properties(test-ts-lib
  PROPERTIES
      FIXTURES_REQUIRED NPM_INSTALL
      ENVIRONMENT APP_CONFIGURATION=$<CONFIG>
  )
endif ()
