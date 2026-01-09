cmake_minimum_required(VERSION 3.10)
project(plazza)

# ------------------------------
# C++ standard & compiler flags
# ------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

# ------------------------------
# Function to enable coverage flags
# ------------------------------
function(enable_coverage TARGET)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${TARGET} PRIVATE -O0 -g --coverage)
        target_link_libraries(${TARGET} PRIVATE --coverage)
    endif()
endfunction()

# ------------------------------
# Source files & includes
# ------------------------------
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Include directories
file(GLOB INCLUDE_DIRS RELATIVE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/*)
foreach(dir ${INCLUDE_DIRS})
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/include/${dir})
        include_directories(${CMAKE_SOURCE_DIR}/include/${dir})
    endif()
endforeach()

# ------------------------------
# Main executable
# ------------------------------
add_executable(plazza main.cpp ${SRC_FILES})
target_include_directories(plazza PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(ENABLE_COVERAGE)
    enable_coverage(plazza)
endif()

# ------------------------------
# Google Test setup
# ------------------------------
enable_testing()

# Find GTest installed by apt (libgtest-dev)
find_package(GTest REQUIRED)
include(GoogleTest)

# ------------------------------
# Test executable
# ------------------------------
file(GLOB_RECURSE TEST_FILES "test/*.cpp")
if(TEST_FILES)
    add_executable(plazza_tests ${SRC_FILES} ${TEST_FILES})
    target_include_directories(plazza_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(plazza_tests PRIVATE GTest::GTest GTest::Main pthread)

    if(ENABLE_COVERAGE)
        enable_coverage(plazza_tests)
    endif()

    # Auto-discover GTest tests
    gtest_discover_tests(plazza_tests)
endif()
