//
// Created by agmui on 12/17/25.
//

#include "MergedBB.h"

namespace merged {

    MergedBB::MergedBB(const std::string &inputFile) {

    }

    int MergedBB::contains(const llvm::Function *F, bool verbose) {
        llvm::GlobalNumberState gn; //TODO: might need to be a field?
        MergedBBComparator mergedBbComparator(headFunc, F, &gn); //TODO: maybe make field
        auto res = mergedBbComparator.compare(BBMap, verbose);
        llvm::outs() << (res ? llvm::raw_ostream::RED : llvm::raw_ostream::GREEN) << "Merged BB Cmp"
                     << llvm::raw_ostream::RESET << '\n';
        return res;
    }

    void MergedBB::merge(const llvm::Function *F, bool verbose) {
        llvm::GlobalNumberState gn; //TODO: might need to be a field?
        MergedBBComparator mergedBbComparator(headFunc, F, &gn); //TODO: maybe make field
        mergedBbComparator.merge(BBMap, verbose);
    }

    void MergedBB::saveToFile(const std::string &filename) const {

    }

} // merged