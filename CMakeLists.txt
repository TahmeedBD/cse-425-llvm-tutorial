cmake_minimum_required(VERSION 3.20)
project(KaleidoscopeTutorial CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find LLVM configuration
find_package(LLVM CONFIG)

if(LLVM_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    include_directories(${LLVM_INCLUDE_DIRS})
    separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
    add_definitions(${LLVM_DEFINITIONS_LIST})
    llvm_map_components_to_libnames(llvm_libs core orcjit native)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/toy.cpp")
    add_executable(toy toy.cpp)
    if(LLVM_FOUND)
        target_link_libraries(toy PRIVATE ${llvm_libs})
    endif()
endif()
