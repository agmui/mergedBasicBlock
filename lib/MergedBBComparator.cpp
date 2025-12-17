//
// Created by agmui on 12/17/25.
//

#include "MergedBBComparator.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/Support/raw_ostream.h"

namespace merged {
    int MergedBBComparator::compare() {

        using namespace llvm; //TODO: remove idk

        beginCompare();

        if (int Res = compareSignature())
            return Res;

        // We do a CFG-ordered walk since the actual ordering of the blocks in the
        // linked list is immaterial. Our walk starts at the entry block for both
        // functions, then takes each block from each terminator in order. As an
        // artifact, this also means that unreachable blocks are ignored.
        SmallVector<const BasicBlock *, 8> FnLBBs, FnRBBs;
        SmallPtrSet<const BasicBlock *, 32> VisitedBBs; // in terms of F1.

        FnLBBs.push_back(&FnL->getEntryBlock());
        FnRBBs.push_back(&FnR->getEntryBlock());

        VisitedBBs.insert(FnLBBs[0]);
        while (!FnLBBs.empty()) {
            const BasicBlock *BBL = FnLBBs.pop_back_val();
            const BasicBlock *BBR = FnRBBs.pop_back_val();

            if (int Res = cmpValues(BBL, BBR))
                return Res;

            if (int Res = cmpBasicBlocks(BBL, BBR))
                return Res;

            const Instruction *TermL = BBL->getTerminator();
            const Instruction *TermR = BBR->getTerminator();

            assert(TermL->getNumSuccessors() == TermR->getNumSuccessors());
            for (unsigned i = 0, e = TermL->getNumSuccessors(); i != e; ++i) {
                if (!VisitedBBs.insert(TermL->getSuccessor(i)).second)
                    continue;

                FnLBBs.push_back(TermL->getSuccessor(i));
                FnRBBs.push_back(TermR->getSuccessor(i));
            }
        }
        return 0;
    }
} // merged