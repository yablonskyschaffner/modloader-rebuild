add_library(std-movies SHARED movies.cpp)
set_target_properties(std-movies PROPERTIES PREFIX "")
target_link_libraries(std-movies PRIVATE modloader_toolkit)
install(TARGETS std-movies RUNTIME DESTINATION "modloader/.data/plugins")
