find_package (PkgConfig)
pkg_check_modules (PC_AMPIFY_E2E QUIET AmpifyE2E)

set (AMPIFY_E2E_ROOT_DIR ${AmpifyE2E_DIR}/../..)
set (AMPIFY_E2E_VERSION 0.0.0)

if (APPLE)
  set (LIBRARY_LOCATION ${AMPIFY_E2E_ROOT_DIR}/lib/libampify-e2e.a)
  set (LIBRARY_LOCATION_DEBUG ${AMPIFY_E2E_ROOT_DIR}/lib/libampify-e2ed.a)
elseif (WIN32)
  set (LIBRARY_LOCATION ${AMPIFY_E2E_ROOT_DIR}/lib/libampify-e2e.lib)
  set (LIBRARY_LOCATION_DEBUG ${AMPIFY_E2E_ROOT_DIR}/lib/libampify-e2ed.lib)
endif ()

mark_as_advanced (AMPIFY_E2E_FOUND AMPIFY_E2E_ROOT_DIR AMPIFY_E2E_VERSION)

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (AmpifyE2E REQUIRED_VARS AMPIFY_E2E_ROOT_DIR
                                   VERSION_VAR AMPIFY_E2E_VERSION)

if (AmpifyE2E_FOUND AND NOT ampify-e2e::ampify-e2e)
  add_library (ampify-e2e::ampify-e2e STATIC IMPORTED)

  target_include_directories (ampify-e2e::ampify-e2e
                              INTERFACE ${AMPIFY_E2E_ROOT_DIR}/include)

  set_target_properties (ampify-e2e::ampify-e2e PROPERTIES IMPORTED_LOCATION
                                                           ${LIBRARY_LOCATION})
  set_target_properties (
    ampify-e2e::ampify-e2e PROPERTIES IMPORTED_LOCATION_DEBUG
                                      ${LIBRARY_LOCATION_DEBUG})
endif ()
