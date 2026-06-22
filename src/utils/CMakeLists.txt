add_library(logging_module logger.cpp)

target_include_directories(logging_module PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

target_link_libraries(logging_module
    PUBLIC
        spdlog::spdlog)

