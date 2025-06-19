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

# Set compiler flags to match original Makefile exactly
set(CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -fno-builtin -std=c++17 -pedantic")

# Get GSL flags using gsl-config
find_program(GSL_CONFIG gsl-config)
if(GSL_CONFIG)
    execute_process(COMMAND ${GSL_CONFIG} --cflags
                   OUTPUT_VARIABLE GSL_CFLAGS
                   OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${GSL_CONFIG} --libs  
                   OUTPUT_VARIABLE GSL_LIBS
                   OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "Found GSL via gsl-config")
    message(STATUS "GSL CFLAGS: ${GSL_CFLAGS}")
    message(STATUS "GSL LIBS: ${GSL_LIBS}")
    
    # Add GSL flags to compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GSL_CFLAGS}")
    
    # Parse libraries from GSL_LIBS
    string(REPLACE " " ";" GSL_LIB_LIST ${GSL_LIBS})
    set(GSL_LIBRARIES ${GSL_LIB_LIST})
else()
    message(WARNING "gsl-config not found")
    set(GSL_LIBRARIES gsl gslcblas)
endif()

# Release flags (equivalent to OFLAGS in Makefile)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -fopenmp")

# Debug flags (equivalent to DEBUGFLAGS in Makefile)  
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O3 -ffast-math")

# Default to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Header files (dependencies)
set(HEADER_FILES
    abstract_quadtree.hpp
    balanced_quadtree.hpp
    vector.hpp
    abstract_fmm_tree.hpp
    balanced_fmm_tree.hpp
    series_expansion.hpp
    multipole_expansion.hpp
    local_expansion.hpp
    fmm_utility.hpp
    fmm_fields.hpp
)

# Define the main target
set(TARGET_NAME integrator_leapfrog_2d)
set(SOURCE_FILE Integration/integrator_leapfrog_2d.cpp)

add_executable(${TARGET_NAME} ${SOURCE_FILE})

# Link libraries using GSL from gsl-config
target_link_libraries(${TARGET_NAME} 
    stdc++fs 
    ${GSL_LIBRARIES}
)

# Make headers as dependencies (for IDE support)
target_sources(${TARGET_NAME} PRIVATE ${HEADER_FILES})

# Custom targets equivalent to Makefile targets

# run target
add_custom_target(run
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
    DEPENDS ${TARGET_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running ${TARGET_NAME}"
)

# valgrind target
add_custom_target(valgrind
    COMMAND valgrind --leak-check=full --track-origins=yes ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
    DEPENDS ${TARGET_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running valgrind on ${TARGET_NAME}"
)

# callgrind target
add_custom_target(callgrind
    COMMAND valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes --collect-atstart=no --callgrind-out-file=./callgrind.out ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
    COMMAND kcachegrind callgrind.out &
    DEPENDS ${TARGET_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running callgrind on ${TARGET_NAME}"
)

# perf target
add_custom_target(perf
    COMMAND perf stat -r 10 ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}
    DEPENDS ${TARGET_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running perf on ${TARGET_NAME}"
)

# clean target (CMake has built-in clean, but this matches your custom clean)
add_custom_target(clean-logs
    COMMAND ${CMAKE_COMMAND} -E remove -f logs/*.dat *.out *.dat callgrind.out
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Cleaning log files and output files"
)