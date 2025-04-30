juce_add_gui_app (e2e-example-app VERSION 0.0.0)

target_sources (
  e2e-example-app PRIVATE source/Application.h source/Main.cpp
                          source/MainComponent.h source/MainWindow.h)

target_include_directories (e2e-example-app PRIVATE source)

target_link_libraries (e2e-example-app PRIVATE focusrite-e2e)

target_compile_definitions (
  e2e-example-app
  PRIVATE
    DONT_SET_USING_JUCE_NAMESPACE=1
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:e2e-example-app,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:e2e-example-app,JUCE_VERSION>"
)

target_link_libraries (
  e2e-example-app PRIVATE juce::juce_gui_extra
  PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags
         juce::juce_recommended_warning_flags)

set_target_properties (e2e-example-app PROPERTIES CXX_STANDARD 17)

add_test(
    NAME e2e-test-example-app
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${NPX_COMMAND} jest "${CMAKE_SOURCE_DIR}/example/tests"
)
set_tests_properties(e2e-test-example-app
PROPERTIES
    FIXTURES_REQUIRED NPM_INSTALL
    ENVIRONMENT APP_CONFIGURATION=$<CONFIG>
)
