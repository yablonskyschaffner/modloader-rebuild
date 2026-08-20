add_library(std-bank SHARED bank.cpp CAECustomBankLoader.cpp)
set_target_properties(std-bank PROPERTIES PREFIX "")
target_link_libraries(std-bank PRIVATE modloader_toolkit)
install(TARGETS std-bank RUNTIME DESTINATION "modloader/.data/plugins")
