cmake_minimum_required(VERSION 2.4)
project(HESS_HP)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Os")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Os")

include_directories(${PROJECT_SOURCE_DIR}/libhess/include)
link_directories(${PROJECT_SOURCE_DIR}/libhess/lib)

add_subdirectory(libhess)

add_executable(hp hp.cpp)
target_link_libraries(hp hess)

add_executable(hc_check validator/hc_check.c validator/hclib.c)