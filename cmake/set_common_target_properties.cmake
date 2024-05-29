function (set_common_target_properties TARGET)

  set_target_properties (${TARGET} PROPERTIES CXX_STANDARD 17)

  target_compile_definitions (${TARGET} PRIVATE $<$<CONFIG:Debug>:DEBUG>)

  if ((${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang") OR (${CMAKE_CXX_COMPILER_ID}
                                                      STREQUAL "AppleClang"))
    target_compile_options (${TARGET} PRIVATE -Wall -Wextra -Werror)
  endif ()
  if (MSVC)
    target_compile_options (${TARGET} PRIVATE /W4 /WX)
  endif ()

endfunction ()
