cmake_minimum_required(VERSION 3.29)
project(llvm_merged_tree)

set(CMAKE_CXX_STANDARD 20)

find_package(LLVM 21.1 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

#  A list of preprocessor defines that should be used when building against LLVM.
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_subdirectory(lib)
add_subdirectory(target)
add_subdirectory(test)
add_subdirectory(tools)

# TODO: =============== debug ===============
add_executable(llvm_merged_tree main.cpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
# run to list all components: $llvm-config --components
llvm_map_components_to_libnames(llvm_libs support core irreader transformutils)
target_link_libraries(llvm_merged_tree PRIVATE ${llvm_libs} MergedBB)#LLVMCore LLVMIRReader LLVMTransformUtils)
target_include_directories(llvm_merged_tree PRIVATE ${LLVM_INCLUDE_DIRS} include)

