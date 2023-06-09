cmake_minimum_required(VERSION 3.14...3.22)

project(SubSimTests LANGUAGES CXX)

# Options
option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

# Import tools
include(../cmake/tools.cmake)

# Dependencies
include(../cmake/CPM.cmake)

CPMAddPackage("gh:doctest/doctest@2.4.9")
CPMAddPackage("gh:TheLartians/Format.cmake@1.7.3")

if(TEST_INSTALLED_VERSION)
    find_package(SubSim REQUIRED)
else()
    CPMAddPackage(NAME SubSim SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)
endif()

# Create binary
add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} doctest::doctest SubSim)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

add_subdirectory(src)

# Enable compiler warnings
if(NOT TEST_INSTALLED_VERSION)
    if(CMAKE_CXX_COMPILER_ID MATCHES "clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(SubSim PUBLIC -Wall -Wpedantic -Wextra -Werror)
    elseif(MSVC)
        target_compile_options(SubSim /W4 /WX)
        target_compile_definitions(${PROJECT_NAME} PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS)
    endif()
endif()

# Add SubSimTests
enable_testing()

# Note: doctest and similar testing frameworks can automatically configure CMake tests. For other testing frameworks,
# add the tests target instead: add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME}
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(${PROJECT_NAME})

# Code coverage
if(ENABLE_TEST_COVERAGE)
    target_compile_options(SubSim PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
    target_link_options(SubSim PUBLIC -fprofile-arcs -ftest-coverage)
endif()
