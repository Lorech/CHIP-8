file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS *.c)
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS ../src/*.c)
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS ../src/*.h)
list(FILTER SOURCE_FILES EXCLUDE REGEX "src/main.c") # Prevent duplicate symbol _main.

add_executable(tests ${TEST_FILES})
target_include_directories(tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${unity_SOURCE_DIR}/src
)

target_link_libraries(tests PRIVATE raylib unity)
add_test(NAME ${PROJECT_NAME}_tests COMMAND tests)
target_sources(tests PRIVATE ${SOURCE_FILES} ${HEADER_FILES} ${TEST_FILES})
