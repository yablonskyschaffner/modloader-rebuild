# The modloader core ASI plugin
add_library(modloader SHARED main.cpp exception.cpp)

# Produces "modloader.asi" (no "lib" prefix, ".asi" suffix)
set_target_properties(modloader PROPERTIES
    PREFIX ""
    SUFFIX ".asi")

target_link_libraries(modloader PRIVATE modloader_toolkit)

# modloader.asi lives at the game root directory
install(TARGETS modloader RUNTIME DESTINATION ".")
