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
  source/TestCentre.cpp
)

target_include_directories (ampify-e2e PUBLIC include)

set (JUCE_MODULES juce_core juce_events)

foreach (JUCE_MODULE ${JUCE_MODULES})
  get_target_property (MODULE_INCLUDES ${JUCE_MODULE}
                       INTERFACE_INCLUDE_DIRECTORIES)
  target_include_directories (ampify-e2e PUBLIC ${MODULE_INCLUDES})
  message ("Adding include directories: ${MODULE_INCLUDES}")
endforeach ()

target_include_directories (ampify-e2e PRIVATE ${juce_SOURCE_DIR}/modules)

set_target_properties (ampify-e2e PROPERTIES CXX_STANDARD 17)

target_compile_definitions (ampify-e2e PRIVATE $<$<CONFIG:Debug>:DEBUG>)
target_compile_definitions (ampify-e2e
                            PRIVATE JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1)

if (APPLE)
  target_compile_options (ampify-e2e PRIVATE -Wall -Wextra -Werror)
  set_target_properties (ampify-e2e PROPERTIES OSX_ARCHITECTURES "arm64;x86_64")
endif()

if (WIN32)
  target_compile_options (ampify-e2e PRIVATE /W4 /WX)
  target_compile_options (ampify-e2e PRIVATE $<IF:$<CONFIG:Debug>,/MTd,/MT>)
endif ()
