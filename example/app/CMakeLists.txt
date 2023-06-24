add_library(juce_modules STATIC)
add_library(focusrite-e2e::juce_modules ALIAS juce_modules)

target_link_libraries(juce_modules PRIVATE juce::juce_gui_extra)

# We're linking the modules privately, but we need to export
# their compile flags
target_compile_definitions(juce_modules
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
    INTERFACE
        $<TARGET_PROPERTY:juce_modules,COMPILE_DEFINITIONS>)

# We also need to export the include directories for the modules
target_include_directories(juce_modules
    INTERFACE
        $<TARGET_PROPERTY:juce_modules,INCLUDE_DIRECTORIES>)

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
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:e2e-example-app,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:e2e-example-app,JUCE_VERSION>"
)

target_link_libraries (
e2e-example-app INTERFACE juce_modules
  PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags
         juce::juce_recommended_warning_flags)

set_target_properties (e2e-example-app PROPERTIES CXX_STANDARD 17)
