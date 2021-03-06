include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(SHP_VERSION_CONFIG "${SHP_GENERATED_CMAKE_DIR}/${PROJECT_NAME}ConfigVersion.cmake")
set(SHP_PROJECT_CONFIG "${SHP_GENERATED_CMAKE_DIR}/${PROJECT_NAME}Config.cmake")

set(SHP_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

set(SHP_TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(SHP_INSTALL_NAMESPACE "${PROJECT_NAME}::")

# TODO: Change to SameMajorVersion when API becomes stable enough
write_basic_package_version_file("${SHP_VERSION_CONFIG}" COMPATIBILITY ExactVersion)
configure_package_config_file(cmake/Config.cmake.in
   "${SHP_PROJECT_CONFIG}"
   INSTALL_DESTINATION "${SHP_INSTALL_CMAKE_DIR}"
)

set(SHP_INSTALL_TARGETS simple_hex_printer)

install(
   TARGETS ${SHP_INSTALL_TARGETS}

   EXPORT ${SHP_TARGETS_EXPORT_NAME}

   RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
           COMPONENT   SHP_Runtime

   LIBRARY DESTINATION        ${CMAKE_INSTALL_LIBDIR}
           COMPONENT          SHP_Runtime
           NAMELINK_COMPONENT SHP_Development

   ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
           COMPONENT   SHP_Development

   INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${SHP_GENERATED_INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(
   FILES
      ${SHP_GENERATED_EXPORT_HEADER}
   DESTINATION
      "${CMAKE_INSTALL_INCLUDEDIR}/shp"
)

install(
   FILES
      ${SHP_PROJECT_CONFIG}
      ${SHP_VERSION_CONFIG}
   DESTINATION
      ${SHP_INSTALL_CMAKE_DIR}
)

set_target_properties(simple_hex_printer PROPERTIES EXPORT_NAME library)
add_library(SimpleHexPrinter::library ALIAS simple_hex_printer)

install(
   EXPORT ${SHP_TARGETS_EXPORT_NAME}
   DESTINATION ${SHP_INSTALL_CMAKE_DIR}
   NAMESPACE ${SHP_INSTALL_NAMESPACE}
   COMPONENT SHP_Development
)
