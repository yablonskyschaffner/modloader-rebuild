add_library(std-rdm SHARED rdm.cpp)
set_target_properties(std-rdm PROPERTIES PREFIX "")
target_link_libraries(std-rdm PRIVATE modloader_toolkit)
install(TARGETS std-rdm RUNTIME DESTINATION "modloader/.data/plugins")
