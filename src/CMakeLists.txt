#
#   San Andreas modloader
#       Modernized CMake script (target-based instead of global flags)
#

# Interface target that carries the common include dirs, C++ standard and
# compiler flags shared by the core and every plugin.
add_library(modloader_toolkit INTERFACE)

target_include_directories(modloader_toolkit INTERFACE
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/Game Structs")

target_compile_features(modloader_toolkit INTERFACE cxx_std_17)

if(MSVC)
    # Don't fight over the buffer-security checks and CRT secure warnings under Release
    target_compile_definitions(modloader_toolkit INTERFACE
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS)

    # We don't want to deal with Buffer Security Check under "Release" build.
    target_compile_options(modloader_toolkit INTERFACE
        $<$<CONFIG:Release>:/GS->)
endif()

if(MINGW)
    # Nothing in this codebase uses exceptions or RTTI
    target_compile_options(modloader_toolkit INTERFACE
        -fno-exceptions
        -fno-rtti)

    # Force static runtime: Windows DLLs and the GNU standard library do not mix well,
    # since the GNU libraries carry different exception models under the same DLL name.
    target_link_options(modloader_toolkit INTERFACE
        -static
        -static-libgcc
        -static-libstdc++)
endif()

#
add_subdirectory(core)
add_subdirectory(plugins)
