//
// Created by agmui on 12/17/25.
//

#ifndef LLVM_MERGED_TREE_MERGEDBBCOMPARATOR_H
#define LLVM_MERGED_TREE_MERGEDBBCOMPARATOR_H

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/Transforms/Utils/FunctionComparator.h"

namespace merged {

class MergedBBComparator : public llvm::FunctionComparator {
    public:
        using FunctionComparator::FunctionComparator;

        int compare();

//    protected:
//        int cmpValues(const Value *L, const Value *R) const;
    };

} // merged

#endif //LLVM_MERGED_TREE_MERGEDBBCOMPARATOR_H
