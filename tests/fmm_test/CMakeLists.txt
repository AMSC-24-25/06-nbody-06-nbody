cmake_minimum_required(VERSION 3.12)
project(FMM_Project)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Try to use g++-8 if available, otherwise use default
find_program(GCC8_COMPILER g++-8)
if(GCC8_COMPILER)
    set(CMAKE_CXX_COMPILER ${GCC8_COMPILER})
    message(STATUS "Using g++-8: ${GCC8_COMPILER}")
endif()

# Set compiler flags to match original Makefile
set(CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -fno-builtin -std=c++17 -pedantic")

# GSL config
find_program(GSL_CONFIG gsl-config)
if(GSL_CONFIG)
    execute_process(COMMAND ${GSL_CONFIG} --cflags
                   OUTPUT_VARIABLE GSL_CFLAGS
                   OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${GSL_CONFIG} --libs  
                   OUTPUT_VARIABLE GSL_LIBS
                   OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "Found GSL via gsl-config")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GSL_CFLAGS}")
    string(REPLACE " " ";" GSL_LIB_LIST ${GSL_LIBS})
    set(GSL_LIBRARIES ${GSL_LIB_LIST})
else()
    message(WARNING "gsl-config not found")
    set(GSL_LIBRARIES gsl gslcblas)
endif()

# Release and debug flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -fopenmp")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O3 -ffast-math")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Header files
set(HEADER_FILES
    abstract_quadtree.hpp
    balanced_quadtree.hpp
    vector.hpp
    particle_generators.hpp
    abstract_fmm_tree.hpp
    balanced_fmm_tree.hpp
    series_expansion.hpp
    multipole_expansion.hpp
    local_expansion.hpp
    fmm_utility.hpp
    fmm_fields.hpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# --- integrator_leapfrog_2d target ---
set(INTEGRATOR_SRC Integration/integrator_leapfrog_2d.cpp)
add_executable(integrator_leapfrog_2d ${INTEGRATOR_SRC})
target_link_libraries(integrator_leapfrog_2d stdc++fs ${GSL_LIBRARIES})
target_sources(integrator_leapfrog_2d PRIVATE ${HEADER_FILES})

# --- accuracy_test target (in Performance folder) ---
set(ACCURACY_SRC Performance/accuracy_test.cpp)
add_executable(accuracy_test ${ACCURACY_SRC})
target_link_libraries(accuracy_test stdc++fs ${GSL_LIBRARIES})
target_sources(accuracy_test PRIVATE ${HEADER_FILES})

# --- efficiency_test target (in Performance folder) ---
set(EFFICIENCY_SRC Performance/efficiency_test.cpp)
add_executable(efficiency_test ${EFFICIENCY_SRC})
target_link_libraries(efficiency_test stdc++fs ${GSL_LIBRARIES})
target_sources(efficiency_test PRIVATE ${HEADER_FILES})

# --- Custom run/valgrind/perf targets for each binary ---
add_custom_target(run
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/integrator_leapfrog_2d
    DEPENDS integrator_leapfrog_2d
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running integrator_leapfrog_2d"
)
add_custom_target(run_accuracy
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/accuracy_test
    DEPENDS accuracy_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running accuracy_test"
)
add_custom_target(run_efficiency
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/efficiency_test
    DEPENDS efficiency_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running efficiency_test"
)

add_custom_target(valgrind
    COMMAND valgrind --leak-check=full --track-origins=yes ${CMAKE_CURRENT_BINARY_DIR}/integrator_leapfrog_2d
    DEPENDS integrator_leapfrog_2d
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Valgrind integrator_leapfrog_2d"
)
add_custom_target(valgrind_accuracy
    COMMAND valgrind --leak-check=full --track-origins=yes ${CMAKE_CURRENT_BINARY_DIR}/accuracy_test
    DEPENDS accuracy_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Valgrind accuracy_test"
)
add_custom_target(valgrind_efficiency
    COMMAND valgrind --leak-check=full --track-origins=yes ${CMAKE_CURRENT_BINARY_DIR}/efficiency_test
    DEPENDS efficiency_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Valgrind efficiency_test"
)

add_custom_target(perf
    COMMAND perf stat -r 10 ${CMAKE_CURRENT_BINARY_DIR}/integrator_leapfrog_2d
    DEPENDS integrator_leapfrog_2d
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Perf integrator_leapfrog_2d"
)
add_custom_target(perf_accuracy
    COMMAND perf stat -r 10 ${CMAKE_CURRENT_BINARY_DIR}/accuracy_test
    DEPENDS accuracy_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Perf accuracy_test"
)
add_custom_target(perf_efficiency
    COMMAND perf stat -r 10 ${CMAKE_CURRENT_BINARY_DIR}/efficiency_test
    DEPENDS efficiency_test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Perf efficiency_test"
)

add_custom_target(clean-logs
    COMMAND ${CMAKE_COMMAND} -E remove -f logs/*.dat *.out *.dat callgrind.out
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Cleaning log files and output files"
)
