add_library (
  ampify-e2e
  source/ampify/e2e/ClickableComponent.h
  source/ampify/e2e/Command.cpp
  source/ampify/e2e/Command.h
  source/ampify/e2e/CommandHandler.h
  source/ampify/e2e/ComponentSearch.cpp
  source/ampify/e2e/ComponentSearch.h
  source/ampify/e2e/Connection.cpp
  source/ampify/e2e/Connection.h
  source/ampify/e2e/DefaultCommandHandler.cpp
  source/ampify/e2e/DefaultCommandHandler.h
  source/ampify/e2e/Event.cpp
  source/ampify/e2e/Event.h
  source/ampify/e2e/KeyPress.cpp
  source/ampify/e2e/KeyPress.h
  source/ampify/e2e/Response.cpp
  source/ampify/e2e/Response.h
  source/ampify/e2e/TestCentre.cpp
  source/ampify/e2e/TestCentre.h)

target_link_libraries (ampify-e2e PUBLIC juce_core juce_events)

target_include_directories (ampify-e2e PUBLIC source)
target_include_directories (ampify-e2e PRIVATE source/private)

set_target_properties (ampify-e2e PROPERTIES CXX_STANDARD 17)

if (APPLE)
  target_compile_options (ampify-e2e PRIVATE -Wall -Wextra -Werror)
elseif (WIN32)
  target_compile_options (ampify-e2e PRIVATE /W4 /WX)
endif ()
