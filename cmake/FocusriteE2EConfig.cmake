find_package (PkgConfig)
pkg_check_modules (PC_FOCUSRITE_E2E QUIET FocusriteE2E)

set (FOCUSRITE_E2E_ROOT_DIR ${FocusriteE2E_DIR}/../..)

if (APPLE)
  set (LIBRARY_LOCATION ${FOCUSRITE_E2E_ROOT_DIR}/lib/libfocusrite-e2e.a)
  set (LIBRARY_LOCATION_DEBUG ${FOCUSRITE_E2E_ROOT_DIR}/lib/libfocusrite-e2ed.a)
elseif (WIN32)
  set (LIBRARY_LOCATION ${FOCUSRITE_E2E_ROOT_DIR}/lib/focusrite-e2e.lib)
  set (LIBRARY_LOCATION_DEBUG ${FOCUSRITE_E2E_ROOT_DIR}/lib/focusrite-e2ed.lib)
endif ()

mark_as_advanced (FOCUSRITE_E2E_FOUND FOCUSRITE_E2E_ROOT_DIR)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (FocusriteE2E
                                   REQUIRED_VARS FOCUSRITE_E2E_ROOT_DIR)

if (FocusriteE2E_FOUND AND NOT focusrite-e2e::focusrite-e2e)
  add_library (focusrite-e2e::focusrite-e2e STATIC IMPORTED)

  target_include_directories (focusrite-e2e::focusrite-e2e
                              INTERFACE ${FOCUSRITE_E2E_ROOT_DIR}/include)

  set_target_properties (focusrite-e2e::focusrite-e2e
                         PROPERTIES IMPORTED_LOCATION ${LIBRARY_LOCATION})
  set_target_properties (
    focusrite-e2e::focusrite-e2e PROPERTIES IMPORTED_LOCATION_DEBUG
                                            ${LIBRARY_LOCATION_DEBUG})
endif ()
