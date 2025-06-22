cmake_minimum_required(VERSION 3.12)
project(FMM_Project CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(GCC8_COMPILER g++-8)
if(GCC8_COMPILER)
    set(CMAKE_CXX_COMPILER ${GCC8_COMPILER})
    message(STATUS "Using g++-8: ${GCC8_COMPILER}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -fno-builtin -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math -fopenmp")
set(CMAKE_CXX_FLAGS_DEBUG "-g -ggdb -O3 -ffast-math")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# GSL config
find_program(GSL_CONFIG gsl-config)
if(GSL_CONFIG)
    execute_process(COMMAND ${GSL_CONFIG} --cflags OUTPUT_VARIABLE GSL_CFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${GSL_CONFIG} --libs OUTPUT_VARIABLE GSL_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "Found GSL via gsl-config")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GSL_CFLAGS}")
    string(REPLACE " " ";" GSL_LIB_LIST ${GSL_LIBS})
    set(GSL_LIBRARIES ${GSL_LIB_LIST})
else()
    message(WARNING "gsl-config not found")
    set(GSL_LIBRARIES gsl gslcblas)
endif()

# Add include dir (modern way)
set(FMM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(integrator_leapfrog_2d Integration/integrator_leapfrog_2d.cpp)
target_include_directories(integrator_leapfrog_2d PRIVATE ${FMM_INCLUDE_DIR})
target_link_libraries(integrator_leapfrog_2d stdc++fs ${GSL_LIBRARIES})

add_executable(accuracy_test Performance/accuracy_test.cpp)
target_include_directories(accuracy_test PRIVATE ${FMM_INCLUDE_DIR})
target_link_libraries(accuracy_test stdc++fs ${GSL_LIBRARIES})

add_executable(efficiency_test Performance/efficiency_test.cpp)
target_include_directories(efficiency_test PRIVATE ${FMM_INCLUDE_DIR})
target_link_libraries(efficiency_test stdc++fs ${GSL_LIBRARIES})

# Custom run/valgrind/perf targets
foreach(target integrator_leapfrog_2d accuracy_test efficiency_test)
    add_custom_target(run_${target}
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${target}
        DEPENDS ${target}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running ${target}"
    )
    add_custom_target(valgrind_${target}
        COMMAND valgrind --leak-check=full --track-origins=yes ${CMAKE_CURRENT_BINARY_DIR}/${target}
        DEPENDS ${target}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Valgrind ${target}"
    )
    add_custom_target(perf_${target}
        COMMAND perf stat -r 10 ${CMAKE_CURRENT_BINARY_DIR}/${target}
        DEPENDS ${target}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Perf ${target}"
    )
endforeach()

add_custom_target(clean-logs
    COMMAND ${CMAKE_COMMAND} -E remove -f logs/*.dat *.out *.dat callgrind.out
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Cleaning log files and output files"
)
