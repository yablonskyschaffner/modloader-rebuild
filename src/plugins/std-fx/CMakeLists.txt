add_library(std-fx SHARED fx.cpp)
set_target_properties(std-fx PROPERTIES PREFIX "")
target_link_libraries(std-fx PRIVATE modloader_toolkit)
install(TARGETS std-fx RUNTIME DESTINATION "modloader/.data/plugins")
