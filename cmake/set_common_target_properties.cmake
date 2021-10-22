function(set_common_target_properties TARGET)

  set_target_properties (${TARGET} PROPERTIES CXX_STANDARD 17)

  target_compile_definitions (${TARGET} PRIVATE $<$<CONFIG:Debug>:DEBUG>)

  if (APPLE)
    target_compile_options (${TARGET} PRIVATE -Wall -Wextra -Werror)
  endif ()

  if (MSVC)
    target_compile_options (${TARGET} PRIVATE /W4 /WX)
    target_compile_options (${TARGET} PRIVATE $<IF:$<CONFIG:Debug>,/MTd,/MT>)
  endif ()

endfunction()