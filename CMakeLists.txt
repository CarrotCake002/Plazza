cmake_minimum_required(VERSION 3.10)
project(plazza)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Enable coverage option
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

# FetchContent for header-only dependencies
include(FetchContent)

# nlohmann/json
FetchContent_Declare(json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Threads (needed by Crow/ASIO)
find_package(Threads REQUIRED)

# ASIO (standalone, needed by Crow — must be populated before Crow)
FetchContent_Declare(asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-28-0
)
FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
  FetchContent_Populate(asio)
endif()
set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include CACHE PATH "ASIO include directory" FORCE)

# Crow (header-only C++ HTTP framework)
FetchContent_Declare(Crow
  GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
  GIT_TAG v1.2.0
)
FetchContent_MakeAvailable(Crow)

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
target_link_libraries(plazza PRIVATE Crow::Crow nlohmann_json::nlohmann_json Threads::Threads)
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
    target_link_libraries(plazza_tests GTest::GTest GTest::Main Crow::Crow nlohmann_json::nlohmann_json Threads::Threads)
    if(ENABLE_COVERAGE)
        enable_coverage(plazza_tests)
    endif()
    gtest_discover_tests(plazza_tests)
endif()
