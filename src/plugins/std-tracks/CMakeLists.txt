add_library(std-tracks SHARED tracks.cpp)
set_target_properties(std-tracks PROPERTIES PREFIX "")
target_link_libraries(std-tracks PRIVATE modloader_toolkit)
install(TARGETS std-tracks RUNTIME DESTINATION "modloader/.data/plugins")
