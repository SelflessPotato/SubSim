cmake_minimum_required(VERSION 3.14...3.22)

project(SubSimApp LANGUAGES CXX)

# Import tools

include(../cmake/tools.cmake)

# Dependencies

include(../cmake/CPM.cmake)

CPMAddPackage(NAME SubSim SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/..)

# Create standalone executable

add_executable(${PROJECT_NAME})
add_subdirectory(src)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17 OUTPUT_NAME "SubSim")
target_link_libraries(${PROJECT_NAME} SubSim)
