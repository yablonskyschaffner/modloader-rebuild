# San Andreas Mod Loader
# Modernized build script (was CMake 2.8-era). Logic preserved, build system modernized.
cmake_minimum_required(VERSION 3.16)

project(modloader
    VERSION 0.1.15
    DESCRIPTION "San Andreas Mod Loader - ASI plugin for GTA San Andreas"
    LANGUAGES C CXX)

# When no build type is specified, use Release (same behavior as the original 2014 build)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build" FORCE)
    message(STATUS "No build type specified, using default: Release")
endif()

set(DOC_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/doc")
set(CONFIGS_DIR "${DOC_DIRECTORY}/config")

# Output directories. Keep the historical layout (bin/ and lib/ next to the source tree)
# so the documented workflow and release scripts keep working.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# DLLs/modules are produced without the usual "lib" prefix (e.g. modloader.asi, std-*.dll)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")

# Build everything (core + plugins)
add_subdirectory(src)

# ----------------------------> Install
# Empty install prefix: install targets directly under <DESTDIR> so that
# "cmake --install . --prefix <game dir>" (or DESTDIR) drops the files where GTA SA expects them.
set(CMAKE_INSTALL_PREFIX "" CACHE PATH "Install path prefix; leave empty to install relative to DESTDIR" FORCE)

# Find all plugins readme files
file(GLOB plugins_doc CONFIGURE_DEPENDS "${DOC_DIRECTORY}/plugins/*.txt")

# Find all readme files
file(GLOB readme_doc CONFIGURE_DEPENDS "${DOC_DIRECTORY}/readme/*.txt")

# Install plugins documentation into the modloader plugins folder
install(FILES ${plugins_doc} DESTINATION "modloader/.data/plugins")

# Install readme files, README.md, LICENSE and CHANGELOG into modloader/.data
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
    "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    "${CMAKE_CURRENT_SOURCE_DIR}/CHANGELOG"
    DESTINATION "modloader/.data")
install(FILES ${readme_doc} DESTINATION "modloader/.data")

# Install config files (not in Debug builds, same as the original)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # Don't copy configs in Debug
else()
    install(FILES "${CONFIGS_DIR}/modloader.ini" DESTINATION "modloader")
    install(FILES "${CONFIGS_DIR}/plugins.ini" DESTINATION "modloader/.data/plugins")
endif()
