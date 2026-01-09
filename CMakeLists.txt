cmake_minimum_required(VERSION 3.10)
project(plazza)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Function to enable coverage flags for a target
function(enable_coverage TARGET)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${TARGET} PRIVATE -O0 -g --coverage)
        target_link_options(${TARGET} PRIVATE --coverage)
    endif()
endfunction()

# Source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Include directories
file(GLOB INCLUDE_DIRS RELATIVE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/*)
foreach(dir ${INCLUDE_DIRS})
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/include/${dir})
        include_directories(${CMAKE_SOURCE_DIR}/include/${dir})
    endif()
endforeach()

# Main executable
add_executable(plazza "main.cpp" ${SRC_FILES})
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
    add_executable(plazza_tests ${TEST_FILES} ${SRC_FILES})
    target_include_directories(plazza_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(plazza_tests GTest::GTest GTest::Main)

    if(ENABLE_COVERAGE)
        enable_coverage(plazza_tests)
    endif()
endif()

# Auto-discover GTest tests
gtest_discover_tests(plazza_tests)
