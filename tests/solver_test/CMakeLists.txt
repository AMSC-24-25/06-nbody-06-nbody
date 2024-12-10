cmake_minimum_required(VERSION 3.30)
project(nbody_solver_test)
find_package(OpenMP)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)

# Include header files
include_directories("../../include/")

# Include source files
file(GLOB SOURCES "../../src/*.cpp")

# Main file name
set(MAIN_FILE "solver_test.cpp")

# Target file name
set(TARGET_FILE "solver_test")

add_executable("${TARGET_FILE}" "${MAIN_FILE}" ${SOURCES})

if(OpenMP_CXX_FOUND)
    target_link_libraries("${TARGET_FILE}" PUBLIC OpenMP::OpenMP_CXX)
endif()