cmake_minimum_required(VERSION 3.18)
project(NBodyCUDA LANGUAGES C CXX CUDA)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable CUDA language
enable_language(CUDA)

# Set CUDA architecture (adjust according to your GPU, e.g., 70 for V100/A100)
set(CMAKE_CUDA_ARCHITECTURES 60 70 75 86)

# Source files
set(SOURCES
    nbody_cuda_leapfrog.cu
    
)

# Header files (for IDE visibility)
set(HEADERS
    vector.hpp
    body.hpp
)
set(CMAKE_CUDA_ARCHITECTURES 70 75 86) # 根据你的显卡设置架构

# Create executable
add_executable(nbody_sim ${SOURCES} ${HEADERS})
add_executable(nbody_sim
    nbody_cuda_leapfrog.cu
)

# CUDA specific settings
set_target_properties(nbody_sim PROPERTIES
    CUDA_SEPARABLE_COMPILATION ON
    CUDA_ARCHITECTURES "${CMAKE_CUDA_ARCHITECTURES}"
    POSITION_INDEPENDENT_CODE ON
)

# Use fast math for performance (optional)
target_compile_options(nbody_sim PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--use_fast_math>)
