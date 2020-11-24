file(GLOB_RECURSE SOURCES "*.cpp" "*.hpp")

add_executable(wadjet_tests ${SOURCES})
target_link_libraries(wadjet_tests PUBLIC wadjet)
add_test(NAME wadjet_tests COMMAND wadjet_tests)