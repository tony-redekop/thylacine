set(TEST_FILES test.cpp)

list(TRANSFORM SOURCE_FILES PREPEND ${CMAKE_SOURCE_DIR}/src/)

add_executable(runtest ${TEST_FILES} ${SOURCE_FILES})
# add_executable(runtest ${TEST_FILES})
target_link_libraries(runtest)