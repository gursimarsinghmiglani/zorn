cmake_minimum_required(VERSION 3.16)
project(ZornCompiler LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
add_executable(ZornCompiler
    src/main.cpp
)
target_include_directories(ZornCompiler PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
llvm_map_components_to_libnames(llvm_libs Core ExecutionEngine Interpreter Support native)
target_link_libraries(ZornCompiler PRIVATE ${llvm_libs})
