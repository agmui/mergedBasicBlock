
add_executable(llvmEquiv llvmEquiv.cpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
# run to list all components: $llvm-config --components
llvm_map_components_to_libnames(llvm_libs support core irreader transformutils)
target_link_libraries(llvmEquiv PRIVATE ${llvm_libs} MergedBB)
target_include_directories(llvmEquiv PRIVATE ${LLVM_INCLUDE_DIRS})

#install(TARGETS llvmEquiv
#  RUNTIME DESTINATION .
#)