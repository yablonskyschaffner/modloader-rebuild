add_library(std-scm SHARED scm.cpp)
set_target_properties(std-scm PROPERTIES PREFIX "")
target_link_libraries(std-scm PRIVATE modloader_toolkit)
install(TARGETS std-scm RUNTIME DESTINATION "modloader/.data/plugins")
