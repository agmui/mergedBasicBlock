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
        MergedBB(const llvm::Function *F) : headFunc(F) {}

        MergedBB(const std::string &inputFile);

        int contains(const llvm::Function *F) const;

        int merge(const llvm::Function *F);

        void saveToFile(const std::string &filename) const;

    private:
        llvm::DenseMap<const llvm::BasicBlock *, const llvm::BasicBlock *> BBMap;
        const llvm::Function *headFunc;
    };

} // merged

#endif //LLVM_MERGED_TREE_MERGEDBB_H
