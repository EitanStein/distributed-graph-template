add_executable(node_tests node_tests.cpp)
add_executable(graph_tests graph_tests.cpp)
add_executable(msg_tests message_tests.cpp)

target_include_directories(graph_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(node_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_include_directories(msg_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(graph_tests
    PRIVATE
        project_options
        core_module
        logging_module
        Catch2::Catch2WithMain
)

target_link_libraries(node_tests
    PRIVATE
        project_options
        core_module
        logging_module
        Catch2::Catch2WithMain
)

target_link_libraries(msg_tests
    PRIVATE
        project_options
        core_module
        logging_module
        Catch2::Catch2WithMain
)

add_test(NAME msg_testing COMMAND $<TARGET_FILE:msg_tests>)
add_test(NAME node_testing COMMAND $<TARGET_FILE:node_tests>)
add_test(NAME graph_testing COMMAND $<TARGET_FILE:graph_tests>)