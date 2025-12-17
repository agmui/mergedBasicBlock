//
// Created by agmui on 12/17/25.
//

#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/Transforms/Utils/FunctionComparator.h"

//https://discourse.llvm.org/t/question-about-instruction-comparison/6534/4
#include "llvm/Transforms/Utils/Cloning.h" //TODO: look into CloneBasicBlock class? it auto makes var mapping?

#include <memory>

//#include "MergedBBComparator.h"
//#include "MergedBB.h"
#include "llvm_merged_tree/include/MergedBBComparator.h"
#include "llvm_merged_tree/include/MergedBB.h"

using namespace llvm;

/// checkModule if the module is alright
Function* checkModule(Module* M){
    bool broken_debug_info = false;
    if (M == nullptr ||
        verifyModule(*M, &errs(), &broken_debug_info)) {
        errs() << "error: module not valid\n";
        return nullptr;
    }
    if (broken_debug_info) {
        errs() << "caution: debug info is broken\n";
    }
    auto F = M->getFunction("string_with_q");
    if (!F) {
        errs() << "error: could not find function 'string_with_q'\n";
        return nullptr;
    }
    return F;
}

int main(int argc, char **argv) {
    //TODO: use llvm args
    if (argc != 3) {
        errs() << "usage: <prog> <IR file>\n";
        return 1;
    }

    // parse an IR file into an LLVM module
    SMDiagnostic Diag;
    LLVMContext C;

    std::unique_ptr<Module> M1 = parseIRFile(argv[1], Diag, C);
    std::unique_ptr<Module> M2 = parseIRFile(argv[2], Diag, C);
    Function *F1 = checkModule(M1.get());
    Function *F2 = checkModule(M2.get());
    if(F1 == nullptr || F2 == nullptr)
        return 1;

    outs() << "iterate instructions of function: '" << F1->getName() << "'\n";

    GlobalNumberState gn;
    merged::MergedBBComparator mergedCmp(F1, F2, &gn);
    outs() << raw_ostream::GREEN << "Merged BB Cmp: " << mergedCmp.compare() << raw_ostream::RESET << '\n';

//    merged::MergedBB mergedBb(F1);
//    mergedBb.contains(F2);
//    mergedBb.merge(F2);
//    mergedBb.saveToFile("mergedBB.bin");
//
//    merged::MergedBB mergedBbReloaded("mergedBB.bin");
//    mergedBbReloaded.contains(F2);

    return 0;
}