//
// Created by agmui on 12/17/25.
//

#ifndef LLVM_MERGED_TREE_MERGEDBBCOMPARATOR_H
#define LLVM_MERGED_TREE_MERGEDBBCOMPARATOR_H

#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "FunctionComparator_cp.h"

namespace merged {

    class MergedBBComparator : public llvm::FunctionComparator_cp {
    public:
        MergedBBComparator(const llvm::Function *F1, const llvm::Function *F2,
                           llvm::GlobalNumberState *GN)
                : llvm::FunctionComparator_cp(F1, F2, GN) {}

        int
        compare(llvm::DenseMap<const llvm::BasicBlock *, std::pair<const llvm::BasicBlock *, llvm::DenseMap<const llvm::Value *, int>>> &BBMap,
                bool verbose = false);

        void
        merge(llvm::DenseMap<const llvm::BasicBlock *, std::pair<const llvm::BasicBlock *, llvm::DenseMap<const llvm::Value *, int>>> &BBMap,
              bool verbose = false);


    protected:
        int cmpBasicBlocks(const llvm::BasicBlock *BBL, const llvm::BasicBlock *BBR) const;


    private:
        enum MatchType {
            allMatch,
            notMatch,
            blockSwitch
        };
        struct MatchResult {
            MatchType matchType;
            union {
                const llvm::BasicBlock *newBB;
                int equiv = 0;
            } value;
        };

        bool mergeBBPath = false;
        bool verbose = false;

        MatchResult recurse(const llvm::BasicBlock *BBL, const llvm::BasicBlock *BBR,
                llvm::SmallPtrSet<const llvm::BasicBlock *, 32> &VisitedBBs,
                llvm::DenseMap<const llvm::BasicBlock *, std::pair<const llvm::BasicBlock *, llvm::DenseMap<const llvm::Value *, int>>> &BBMap,
                const llvm::BasicBlock *BBRParent = nullptr);
    };

} // merged

#endif //LLVM_MERGED_TREE_MERGEDBBCOMPARATOR_H
