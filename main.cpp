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
#include <string>
#include <iostream>
#include <filesystem>
#include <set>

#include "MergedBBComparator.h"
#include "MergedBB.h"

namespace fs = std::filesystem;
using namespace llvm;

/// checkModule if the module is alright
Function* checkModule(Module* M, StringRef functionName){
    bool broken_debug_info = false;
    if (M == nullptr ||
        verifyModule(*M, &errs(), &broken_debug_info)) {
        errs() << "error: module not valid\n";
        return nullptr;
    }
    if (broken_debug_info) {
        errs() << "caution: debug info is broken\n";
    }
    auto F = M->getFunction(functionName);
    if (!F) {
        errs() << "error: could not find function 'string_with_q'\n";
        return nullptr;
    }
    return F;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        errs() << "usage: llvm_merged_tree <path to IR dir>\n";
        return 1;
    }

    // parse an IR file into an LLVM module
    SMDiagnostic Diag;
    LLVMContext C;

    std::vector<std::unique_ptr<Module>> moduleArr;
    std::vector<Function*> funcArr;
    std::set<fs::path> sorted_by_name;
    for (const auto & entry : fs::directory_iterator(argv[1]))
        sorted_by_name.insert(entry.path());
    for (auto &filename:sorted_by_name) {
//        std::cout << filename << std::endl;
        std::unique_ptr<Module> M = parseIRFile(filename.c_str(), Diag, C);
        Function *F1 = checkModule(M.get(), "string_with_q");
        if(F1 == nullptr)
            return 1;
        funcArr.push_back(F1);
        moduleArr.emplace_back(std::move(M));
    }
//    outs() << "\n";


    // ---------------------------------------------------------------------------------------------------------



    Function *s1 = funcArr[10];
    Function *s17 = funcArr[7];
    outs() << "iterate instructions of function: '" << s1->getName() << "'\n";

    GlobalNumberState gn;
    merged::MergedBBComparator mergedCmp(s1, s17, &gn);
    outs() << raw_ostream::GREEN << "Merged BB Cmp: " << mergedCmp.compare() << raw_ostream::RESET << '\n';

//    merged::MergedBB mergedBb(F1);
//    mergedBb.contains(F2);
//    mergedBb.merge(F2);
//    mergedBb.saveToFile("mergedBB.bin");
//
//    merged::MergedBB mergedBbReloaded("mergedBB.bin");
//    mergedBbReloaded.contains(F2);

/*
    for(auto [BB1, BB2] : zip(*F1, *F2)) {
//        outs() << BB1.getName();
//        outs() << BB2.getName();
        for (auto [Inst1, Inst2]: zip(BB1, BB2)) {
            outs() << "----\n";
            Inst1.print(outs());
            outs() << '\n';
            Inst2.print(outs());
            outs() << '\n';

            //TODO: checkout cmpBasicBlocks() and cmpOperations() in FunctionComparator.cpp
            //https://llvm.org/doxygen/classllvm_1_1FunctionComparator.html

            bool isSameResult = Inst1.isSameOperationAs(&Inst2);
            bool isSameResultIgnoreAlignment = Inst1.isSameOperationAs(&Inst2, Instruction::OperationEquivalenceFlags::CompareIgnoringAlignment);
            bool isSameResultScalarTypes = Inst1.isSameOperationAs(&Inst2, Instruction::OperationEquivalenceFlags::CompareUsingIntersectedAttrs);
            bool isSameResultIntersect = Inst1.isSameOperationAs(&Inst2, Instruction::OperationEquivalenceFlags::CompareUsingScalarTypes);
            auto allFlags = Instruction::OperationEquivalenceFlags::CompareIgnoringAlignment | Instruction::OperationEquivalenceFlags::CompareUsingIntersectedAttrs | Instruction::OperationEquivalenceFlags::CompareUsingScalarTypes;
            bool isSameResultAllFlags = Inst1.isSameOperationAs(&Inst2, allFlags);
            outs()
                << "isSame: "<< (isSameResult ? "equiv" : "not equiv") << "\n"
                << "ignoreAlignment: "<< (isSameResultIgnoreAlignment ? "equiv" : "not equiv") << "\n"
                << "ScalarTypes: "<< (isSameResultScalarTypes ? "equiv" : "not equiv") << "\n"
                << "Intersect: "<< (isSameResultIntersect ? "equiv" : "not equiv") << "\n"
                << "allFlags: " << (isSameResultAllFlags ? "equiv" : "not equiv")
                << '\n';
        }
    }
*/


/*
    for (auto &BB : *F1) {
        for (auto &I : BB) {
            I.print(outs());

//            I.isSameOperationAs()
            StringRef name = I.getOpcodeName();

//            llvm::Function::viewCFG();

//            llvm::ValueMap
            outs() << '\n';
            // TODO: Analyze instruction 'I' here.
            // (see http://llvm.org/doxygen/classllvm_1_1Instruction.html)
            // For instance, let's check if 'I' is an 'AllocaInst':
            //
            // if (auto Alloc = llvm::dyn_cast<llvm::AllocaInst>(&I)) {
            //    At this point you can use the memberfunctions of llvm::AllocaInst
            //    on the pointer variable Alloc as it is a more specialized sub-type
            //    of llvm::Instruction.
            //    llvm::outs() << "Found an alloca instruction!\n";
            // }
            //
            // For the further tasks you may like to familiarize yourself with the
            // sub-instructions 'llvm::LoadInst' and 'llvm::StoreInst' as well as
            // 'llvm::CallInst'. Use if-constructs like shown in the above.
            //
            // If you just want to figure out if a variable is an instance of a
            // certain type you can use 'llvm::isa<>()' like this:
            //
            // if (llvm::isa<llvm::CallInst>(&I)) {
            //   llvm::outs() << "Found a CallInst!\n";
            // }
            //
            // Both, 'llvm::isa' and 'llvm::dyn_cast' only work for variables within
            // the universe of LLVM. These features have been crafted for efficiency
            // and only require a single look-up; so do not be afraid to use them.
            // If you need to find inheritance relationsships in non-LLVM context use
            // C++'s classical 'dynamic_cast'.
        }
    }
*/
    return 0;
}
