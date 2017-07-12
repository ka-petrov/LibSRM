cmake_minimum_required(VERSION 3.7)
project(libsrm)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES srm.cpp srm.h)
set(TEST_SOURCE_FILES srm_test.cpp)

add_library(srm SHARED ${SOURCE_FILES})
add_executable(srm_test ${TEST_SOURCE_FILES})
target_link_libraries(srm_test srm)