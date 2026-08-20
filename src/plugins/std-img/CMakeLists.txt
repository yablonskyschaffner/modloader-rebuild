# std-img needs low-level GAS assembly on MinGW (backend.s) and a C file with
# inline asm on MSVC (backend.c). Enable the ASM language only where needed.
if(MINGW)
    enable_language(ASM)
    set(ASM_EXT "s")
elseif(MSVC)
    set(ASM_EXT "c")
endif()

add_library(std-img SHARED img.cpp streaming.cpp backend.${ASM_EXT})
set_target_properties(std-img PROPERTIES PREFIX "")
target_link_libraries(std-img PRIVATE modloader_toolkit)
install(TARGETS std-img RUNTIME DESTINATION "modloader/.data/plugins")
