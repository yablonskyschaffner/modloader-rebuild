add_library(std-data SHARED data.cpp ipl.cpp handling.cpp)
set_target_properties(std-data PROPERTIES PREFIX "")
target_link_libraries(std-data PRIVATE modloader_toolkit)
install(TARGETS std-data RUNTIME DESTINATION "modloader/.data/plugins")
