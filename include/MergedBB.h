//
// Created by agmui on 12/17/25.
//

#ifndef LLVM_MERGED_TREE_MERGEDBB_H
#define LLVM_MERGED_TREE_MERGEDBB_H

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "MergedBBComparator.h"


namespace merged {

    class MergedBB {
    public:
        //TODO: check if we need to merge to setup the BBMap with the headFunc
        MergedBB(const llvm::Function *F) : headFunc(F) { merge(F); }

        MergedBB(const std::string &inputFile);

        int contains(const llvm::Function *F, bool verbose = false);

        void merge(const llvm::Function *F, bool verbose = false);

        void saveToFile(const std::string &filename) const;

    private:
        llvm::DenseMap<const llvm::BasicBlock *, std::pair<const llvm::BasicBlock *, llvm::DenseMap<const llvm::Value *, int>>> BBMap;
        const llvm::Function *headFunc;
    };

} // merged

#endif //LLVM_MERGED_TREE_MERGEDBB_H
