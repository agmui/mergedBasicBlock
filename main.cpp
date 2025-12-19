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
Function *checkModule(Module *M, StringRef functionName) {
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
    std::vector<std::pair<Function *, std::string>> funcArr;
    std::set<fs::path> sorted_by_name;
    for (const auto &entry: fs::directory_iterator(argv[1]))
        sorted_by_name.insert(entry.path());
    for (auto &filename: sorted_by_name) {
//        std::cout << filename << std::endl;
        std::unique_ptr<Module> M = parseIRFile(filename.c_str(), Diag, C);
        Function *F1 = checkModule(M.get(), "string_with_q");
        if (F1 == nullptr)
            return 1;
        auto s = filename.string();
        std::string sampleName = "studentXX_c_dbg.ll";
        funcArr.emplace_back(F1, s.substr(s.size() - sampleName.size(), sampleName.size()));
        moduleArr.emplace_back(std::move(M));
    }
//    outs() << "\n";


    // ---------------------------------------------------------------------------------------------------------



//    Function *s1 = funcArr[10].first;
//    Function *s17 = funcArr[7].first;
//
//    merged::MergedBB mergedBb(s1);
//    mergedBb.contains(s17);
//    mergedBb.merge(s17);
//    mergedBb.contains(s17);
//    mergedBb.saveToFile("mergedBB.bin");
//    merged::MergedBB mergedBbReloaded("mergedBB.bin");
//    mergedBbReloaded.contains(s17);
//
//    auto s10 = funcArr[0].first;
//    auto s15 = funcArr[5].first;
//    merged::MergedBB mergedBb(s10);
//    mergedBb.contains(s10);
//    mergedBb.merge(s10);
//    mergedBb.contains(s10);
//    outs() << "-- s15 --" << "\n";
//    mergedBb.contains(s15);
//    mergedBb.merge(s15);
//    mergedBb.contains(s15, true);

    outs() << "\n\nmulti file test:" << "\n";
    merged::MergedBB mergedBb2(funcArr[0].first);
    for (auto &[f, filename]: funcArr) {
        outs() << filename << '\n';
        mergedBb2.contains(f);
        mergedBb2.merge(f);
        mergedBb2.contains(f);
        outs() << "---" << "\n";
    }

    return 0;
}
