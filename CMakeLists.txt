cmake_minimum_required(VERSION 3.10)
project(plazza)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

function(enable_coverage TARGET)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${TARGET} PRIVATE -O0 -g --coverage)
        target_link_options(${TARGET} PRIVATE --coverage)
    endif()
endfunction()

# Find all .cpp files in src/ and subdirectories
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Find all subdirectories in the include directory
file(GLOB INCLUDE_DIRS RELATIVE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/*)

# Add the include directory and all its subdirectories
foreach(dir ${INCLUDE_DIRS})
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/include/${dir})
        include_directories(${CMAKE_SOURCE_DIR}/include/${dir})
    endif()
endforeach()

# Define the executable
add_executable(plazza "main.cpp" ${SRC_FILES})

# Ensure the compiler searches the include directory
target_include_directories(plazza PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(ENABLE_COVERAGE)
    enable_coverage(plazza)
endif()

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
endif()

gtest_discover_tests(plazza_tests)