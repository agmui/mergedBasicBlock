//
// Created by agmui on 12/17/25.
//

#include "MergedBB.h"

namespace merged {

    MergedBB::MergedBB(const std::string &inputFile) {

    }

    int MergedBB::contains(const llvm::Function *F) const {
        llvm::GlobalNumberState gn;
        MergedBBComparator mergedBbComparator(headFunc, F, &gn); //TODO: maybe make field
        return mergedBbComparator.compare();
    }

    int MergedBB::merge(const llvm::Function *F) {
        return 0;
    }

    void MergedBB::saveToFile(const std::string &filename) const {

    }
} // merged