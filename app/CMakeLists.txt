add_library (
  ampify-e2e
  include/ampify/e2e/ClickableComponent.h
  include/ampify/e2e/Command.h
  include/ampify/e2e/CommandHandler.h
  include/ampify/e2e/ComponentSearch.h
  include/ampify/e2e/Event.h
  include/ampify/e2e/Response.h
  include/ampify/e2e/TestCentre.h
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

add_library (ampify-e2e::ampify-e2e ALIAS ampify-e2e)

target_include_directories (ampify-e2e PUBLIC include)

set (JUCE_MODULES juce_core juce_events)

foreach (JUCE_MODULE ${JUCE_MODULES})
  get_target_property (MODULE_INCLUDES ${JUCE_MODULE}
                       INTERFACE_INCLUDE_DIRECTORIES)
  target_include_directories (ampify-e2e PUBLIC ${MODULE_INCLUDES})
endforeach ()

target_include_directories (ampify-e2e PRIVATE ${juce_SOURCE_DIR}/modules)

target_compile_definitions (ampify-e2e
                            PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)

set_common_target_properties (ampify-e2e)

get_target_property (AMPIFY_E2E_SOURCES ampify-e2e SOURCES)
source_group (TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${AMPIFY_E2E_SOURCES})

if (AMPIFY_E2E_MAKE_TESTS)

  add_executable (ampify-e2e-tests ./tests/TestCommand.cpp
                                   ./tests/TestResponse.cpp)

  target_link_libraries (ampify-e2e-tests PRIVATE Catch2::Catch2WithMain
                                                  ampify-e2e)

  add_test (NAME ampify-e2e-tests COMMAND ampify-e2e-tests)

  set_common_target_properties (ampify-e2e-tests)

  target_compile_definitions (
    ampify-e2e-tests PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
                             JUCE_STANDALONE_APPLICATION=1)

  target_link_libraries (
    ampify-e2e-tests PRIVATE juce::juce_core juce::juce_events juce::juce_gui_basics
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags)
endif ()
