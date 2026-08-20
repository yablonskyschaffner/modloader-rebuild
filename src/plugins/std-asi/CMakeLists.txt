add_library(std-asi SHARED asi.cpp CsInfo.cpp ModuleInfo.cpp)
set_target_properties(std-asi PROPERTIES PREFIX "")
target_link_libraries(std-asi PRIVATE modloader_toolkit)
install(TARGETS std-asi RUNTIME DESTINATION "modloader/.data/plugins")
