add_library(std-cleo SHARED cleo.cpp)
set_target_properties(std-cleo PROPERTIES PREFIX "")
target_link_libraries(std-cleo PRIVATE modloader_toolkit)
install(TARGETS std-cleo RUNTIME DESTINATION "modloader/.data/plugins")
