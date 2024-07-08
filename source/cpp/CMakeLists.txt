add_library (focusrite-e2e STATIC
  source/Command.cpp
  source/ComponentSearch.cpp
  source/Connection.cpp
  source/Connection.h
  source/DefaultCommandHandler.cpp
  source/DefaultCommandHandler.h
  source/Event.cpp
  source/KeyPress.cpp
  source/KeyPress.h
  source/Response.cpp
  source/TestCentre.cpp)

add_library (focusrite-e2e::focusrite-e2e ALIAS focusrite-e2e)

target_include_directories (focusrite-e2e PUBLIC include)

target_link_libraries(focusrite-e2e PUBLIC focusrite-e2e::juce_modules)

set_common_target_properties (focusrite-e2e)

get_target_property (FOCUSRITE_E2E_SOURCES focusrite-e2e SOURCES)
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FOCUSRITE_E2E_SOURCES})

if (FOCUSRITE_E2E_MAKE_TESTS)

  add_executable (
    focusrite-e2e-tests
    ./tests/main.cpp ./tests/TestCommand.cpp ./tests/TestComponentSearch.cpp
    ./tests/TestResponse.cpp)

  target_link_libraries (focusrite-e2e-tests PRIVATE focusrite-e2e)

  add_test (NAME focusrite-e2e-tests COMMAND focusrite-e2e-tests)

  set_common_target_properties (focusrite-e2e-tests)

  target_compile_definitions (
    focusrite-e2e-tests PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
                                JUCE_STANDALONE_APPLICATION=1)

  target_link_libraries (
    focusrite-e2e-tests PRIVATE juce::juce_core juce::juce_events
                                juce::juce_gui_basics
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags)

  set_target_properties (focusrite-e2e-tests
      PROPERTIES
      WIN32_EXECUTABLE true)

endif ()
