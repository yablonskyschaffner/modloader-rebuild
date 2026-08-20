add_library(std-text SHARED text.cpp)
set_target_properties(std-text PROPERTIES PREFIX "")
target_link_libraries(std-text PRIVATE modloader_toolkit)
install(TARGETS std-text RUNTIME DESTINATION "modloader/.data/plugins")
