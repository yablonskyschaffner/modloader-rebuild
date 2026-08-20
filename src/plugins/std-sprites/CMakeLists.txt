add_library(std-sprites SHARED sprites.cpp)
set_target_properties(std-sprites PROPERTIES PREFIX "")
target_link_libraries(std-sprites PRIVATE modloader_toolkit)
install(TARGETS std-sprites RUNTIME DESTINATION "modloader/.data/plugins")
