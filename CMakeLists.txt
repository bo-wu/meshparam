cmake_minimum_required(VERSION 2.8)
project(parameterize)

file(GLOB headers "${CMAKE_SOURCE_DIR}/src/*.h")
file(GLOB sources "${CMAKE_SOURCE_DIR}/src/*.cpp")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(openmesh OpenMeshCore)# OpenMeshTools)
set(cgal CGAL)

set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_RELEASE " ${CMAKE_CXX_FLAGS_RELEASE} -Wreturn-type -fopenmp")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -Wno-sign-compare")
message("CMAKE_CXX_FLAGS_RELEASE is " ${CMAKE_CXX_FLAGS_RELEASE})

include_directories(/usr/local/include/eigen3/ /usr/local/include/)
#link_directories(/home/wubo/workspace/darwin/bin/)

add_executable(param ${headers} ${sources})
target_link_libraries(param ${cgal} ${openmesh})
