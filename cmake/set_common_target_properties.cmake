function (set_common_target_properties TARGET)
  set_target_properties (${TARGET} PROPERTIES CXX_STANDARD 17)
  target_compile_definitions (${TARGET} PRIVATE $<$<CONFIG:Debug>:DEBUG>)
  target_link_libraries (${TARGET}
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
  )

  if (APPLE)
    target_compile_options (${TARGET} PRIVATE -Wextra -Werror)
  endif ()

  if (MSVC)
    target_compile_options (${TARGET} PRIVATE /WX)
  endif ()
endfunction ()
