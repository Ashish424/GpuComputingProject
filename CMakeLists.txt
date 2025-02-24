cmake_minimum_required(VERSION 3.6)
project(GpuProject)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${PROJECT_SOURCE_DIR}/bin)


find_package(CUDA REQUIRED)
MESSAGE( STATUS "cuda toolkit location" ${CUDA_TOOLKIT_ROOT_DIR} )




set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/glfw)



include_directories(
        "deps/glad/include"
        "deps/glfw/include"
        "deps/glm"
        "deps/imgui"
        "deps/tinyobjloader"
        "deps/assimp/include"
        "deps/glraw/source/glraw")

file(GLOB DEPS_SOURCES deps/glad/src/glad.c)
file(GLOB TINY_OBJ_SOURCE deps/tinyobjloader/*)
file(GLOB IMGUI_SOURCE
        deps/imgui/*.h
        deps/imgui/*.cpp
        )


set(DEPS_SOURCES ${DEPS_SOURCES})

if(CUDA_FOUND)
    #TODO see these flags

    set(CUDA_NVCC_FLAGS "-std=c++11")
    if(UNIX)
        set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -ccbin /usr/bin/gcc-4.9;")
    endif(UNIX)
    set(CUDA_NVCC_FLAGS "
    ${CUDA_NVCC_FLAGS}
#  -gencode;arch=compute_30,code=sm_30;
#  -gencode;arch=compute_35,code=sm_35;
#  -gencode;arch=compute_35,code=compute_35;
#  -gencode;arch=compute_20,code=sm_20;
#  -gencode;arch=compute_11,code=sm_11;
#  -gencode;arch=compute_12,code=sm_12;
#  -gencode;arch=compute_13,code=sm_13;
")
#TODO off them in release build
# add debugging to CUDA NVCC flags.  For NVidia's NSight tools.
set(CUDA_NVCC_FLAGS_DEBUG ${CUDA_NVCC_FLAGS_DEBUG} " -G")


else(CUDA_FOUND)
    message("CUDA is not installed on this system.")
endif()


set(PLATFORM_LINK_GLAD)
if (UNIX)
    set(PLATFORM_LINK_GLAD -ldl)
endif (UNIX)

add_subdirectory(CpuRayTracer)
add_subdirectory(GpuPathTracer)


