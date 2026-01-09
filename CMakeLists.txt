cmake_minimum_required(VERSION 3.10)
project(plazza)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Enable coverage option
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

# Function to add coverage flags to a target
function(enable_coverage TARGET)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${TARGET} PRIVATE -O0 -g --coverage)
        target_link_options(${TARGET} PRIVATE --coverage)
    endif()
endfunction()

# Source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Main executable
add_executable(plazza main.cpp ${SRC_FILES})
target_include_directories(plazza PRIVATE ${CMAKE_SOURCE_DIR}/include)
if(ENABLE_COVERAGE)
    enable_coverage(plazza)
endif()

# Enable testing
enable_testing()
include(GoogleTest)

# Google Test
find_package(GTest REQUIRED)

# Test executable
file(GLOB_RECURSE TEST_FILES "test/*.cpp")
if(TEST_FILES)
    add_executable(plazza_tests ${SRC_FILES} ${TEST_FILES})
    target_include_directories(plazza_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(plazza_tests GTest::GTest GTest::Main)
    if(ENABLE_COVERAGE)
        enable_coverage(plazza_tests)
    endif()
    gtest_discover_tests(plazza_tests)
endif()
