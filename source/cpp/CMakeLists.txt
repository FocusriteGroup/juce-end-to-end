add_library (
  focusrite-e2e
  include/focusrite/e2e/ClickableComponent.h
  include/focusrite/e2e/Command.h
  include/focusrite/e2e/CommandHandler.h
  include/focusrite/e2e/ComponentSearch.h
  include/focusrite/e2e/Event.h
  include/focusrite/e2e/Response.h
  include/focusrite/e2e/TestCentre.h
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

set (JUCE_MODULES juce_core juce_events)

foreach (JUCE_MODULE ${JUCE_MODULES})
 
  if (NOT TARGET ${JUCE_MODULE})
    message(FATAL_ERROR "Missing JUCE module: ${JUCE_MODULE}, enable FOCUSRITE_E2E_FETCH_JUCE to fetch JUCE")
  endif()

  get_target_property (MODULE_INCLUDES ${JUCE_MODULE}
                       INTERFACE_INCLUDE_DIRECTORIES)
  target_include_directories (focusrite-e2e PUBLIC ${MODULE_INCLUDES})
endforeach ()

target_include_directories (focusrite-e2e PRIVATE ${juce_SOURCE_DIR}/modules)

target_compile_definitions (focusrite-e2e
                            PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)

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
