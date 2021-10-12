add_library (
  ampify-e2e
  ./source/ampify/e2e/Connection.h
  ./source/ampify/e2e/Response.h
  ./source/ampify/e2e/Event.h
  ./source/ampify/e2e/Command.cpp
  ./source/ampify/e2e/TestCentre.h
  ./source/ampify/e2e/Response.cpp
  ./source/ampify/e2e/TestCentre.cpp
  ./source/ampify/e2e/Command.h
  ./source/ampify/e2e/Event.cpp
  ./source/ampify/e2e/Connection.cpp)

target_link_libraries (ampify-e2e PUBLIC juce_core juce_events)

set_target_properties (ampify-e2e PROPERTIES CXX_STANDARD 17)
