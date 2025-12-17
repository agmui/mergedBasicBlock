add_custom_target(LLFileGeneration ALL)

# Use phasar's capabilities to automate the LLVM-IR file generation
include(../phasar_macros.cmake)

file(GLOB target_files RELATIVE ${CMAKE_CURRENT_LIST_DIR} clab/*.c)

set(PHASAR_LLVM_VERSION 21)
foreach(target_file ${target_files})
  generate_ll_file(FILE ${target_file} DEBUG)
  #clang -g -grecord-command-line -fno-inline-functions -fno-builtin -O3 -S -emit-llvm "$file" -o "${file%.c}.ll"
endforeach()