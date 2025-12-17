
add_library(MergedBB MergedBBComparator.cpp MergedBB.cpp)

llvm_map_components_to_libnames(llvm_libs support core irreader transformutils)
target_link_libraries(MergedBB PRIVATE ${llvm_libs})#LLVMCore LLVMIRReader LLVMTransformUtils)
target_include_directories(MergedBB PRIVATE ${LLVM_INCLUDE_DIRS} ../include)
