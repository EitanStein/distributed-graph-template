add_library(core_module node.cpp)

target_include_directories(core_module PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

target_link_libraries(core_module
    PRIVATE
        logging_module)