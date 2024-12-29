file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS *.c)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${unity_SOURCE_DIR}/src
)

foreach(TEST_FILE ${TEST_FILES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)

    # Link the source file to the test file.
    string(REPLACE "test_" "" MODULE_NAME ${TEST_NAME})
    set(SRC_FILE ${CMAKE_SOURCE_DIR}/src/${MODULE_NAME}.c)

    # Collect additional dependencies for integrated tests.
    # TODO: See if this can somehow be automated in the future.
    set(DEPENDENCIES)
    if(${TEST_NAME} STREQUAL "test_cpu")
        list(APPEND DEPENDENCIES ${CMAKE_SOURCE_DIR}/src/display.c)
        list(APPEND DEPENDENCIES ${CMAKE_SOURCE_DIR}/src/memory.c)
        list(APPEND DEPENDENCIES ${CMAKE_SOURCE_DIR}/src/stack.c)
    endif()

    add_executable(${TEST_NAME} ${TEST_FILE} ${SRC_FILE} ${DEPENDENCIES})
    add_test(NAME ${PROJECT_NAME}_${TEST_NAME} COMMAND ${TEST_NAME})
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${TEST_NAME} PRIVATE raylib unity)
    target_compile_definitions(${TEST_NAME} PRIVATE -DUNIT_TEST)

    add_custom_command(
        TARGET ${TEST_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${TEST_NAME}>/resources
    )
endforeach()
