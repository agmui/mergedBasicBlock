//
// Created by agmui on 12/17/25.
//

#include "MergedBBComparator.h"
#include "llvm/Support/raw_ostream.h"

// debug logging macros
#define LOG(x) if(verbose) outs() << x << "\n";
#define ERR(x) if(verbose) outs() << raw_ostream::RED << x << raw_ostream::RESET << "\n";

using namespace llvm;

namespace merged {

    int MergedBBComparator::cmpBasicBlocks(const BasicBlock *BBL, const BasicBlock *BBR) const {
        BasicBlock::const_iterator InstL = BBL->begin(), InstLE = BBL->end();
        BasicBlock::const_iterator InstR = BBR->begin(), InstRE = BBR->end();

        do {
            if (verbose) {
                InstL->print(outs());
                outs() << "\n";
                InstR->print(outs());
                outs() << "\n\n";
            }
            bool needToCmpOperands = true;
            if (int Res = cmpOperations(&*InstL, &*InstR, needToCmpOperands)) {
                ERR("not same op code")
                return Res;
            }
            if (needToCmpOperands) {
                assert(InstL->getNumOperands() == InstR->getNumOperands());

                for (unsigned i = 0, e = InstL->getNumOperands(); i != e; ++i) {
                    Value *OpL = InstL->getOperand(i);
                    Value *OpR = InstR->getOperand(i);
                    if (int Res = cmpValues(OpL, OpR)) {
                        ERR("not same value: " << OpL->getName() << ", " << OpR->getName())
                        return Res;
                    }
                    // cmpValues should ensure this is true.
                    assert(cmpTypes(OpL->getType(), OpR->getType()) == 0);
                }
            }

            ++InstL;
            ++InstR;
        } while (InstL != InstLE && InstR != InstRE);

        if (InstL != InstLE && InstR == InstRE) {
            ERR("BB len dont match")
            return 1;
        }
        if (InstL == InstLE && InstR != InstRE) {
            ERR("BB len dont match")
            return -1;
        }
        return 0;
    }

    MergedBBComparator::MatchResult
    MergedBBComparator::recurse(const BasicBlock *BBL, const BasicBlock *BBR,
                                SmallPtrSet<const BasicBlock *, 32> &VisitedBBs,
                                DenseMap<const BasicBlock *, std::pair<const BasicBlock *, DenseMap<const Value *, int>>> &BBMap,
                                const BasicBlock *BBRParent) {

        LOG("---")

        LOG(BBL << " " << BBL->getName() << " | " << BBR << " " << BBR->getName())
        bool usingAlt = false;
        // check if BBL == BBR
        for (int cmpValuesErr, cmpBBErr;
             (cmpValuesErr = cmpValues(BBL, BBR)) || (cmpBBErr = cmpBasicBlocks(BBL, BBR));) {
            // if not search for an alt
            if (cmpValuesErr) ERR("cmpValues() err: " << cmpValuesErr)
            LOG("using alt")
            if (!BBMap.contains(BBL)) {// if check for alt
                if (mergeBBPath) {
                    BBMap.insert(std::make_pair(BBL, std::make_pair(BBR, sn_mapR)));
                } else {
                    //TODO: add custom smt eval here
                }
                ERR("out of alts")
                return {notMatch, {.equiv= cmpValuesErr ? cmpValuesErr : cmpBBErr}}; // if we have run out of alts
            }

            auto &mapRes = BBMap.at(BBL); // switch to alt
            BBL = mapRes.first;
            sn_mapL = mapRes.second; //TODO: maybe consider move?
            usingAlt = true;
        }

        const Instruction *TermL = BBL->getTerminator();
        const Instruction *TermR = BBR->getTerminator();

        assert(TermL->getNumSuccessors() == TermR->getNumSuccessors());
        for (unsigned i = 0, e = TermL->getNumSuccessors(); i != e; ++i) {
            if (!VisitedBBs.insert(TermR->getSuccessor(i)).second)
                continue;

            auto [matchType, value] = recurse(TermL->getSuccessor(i), TermR->getSuccessor(i), VisitedBBs, BBMap, BBR);
            switch (matchType) {
                case allMatch: // Do nothing continue for loop
                    break;
                case notMatch: // exit early
                    return {notMatch, value};
                case blockSwitch:
                    TermL = value.newBB->getTerminator();
                    //TODO: handel case if TermL has reached end of loop because of the alt switch
                    assert(TermL->getNumSuccessors() == TermR->getNumSuccessors());
                    usingAlt = true;
                    BBL = value.newBB;
                    break;
            }
        }
        if (usingAlt) {
            // check if BBL is starting block and just return nullptr
            if (BBL->isEntryBlock()) return {blockSwitch, nullptr};

            auto pred = BBL->getSinglePredecessor();
            if (pred == nullptr) {
                ERR("multiple predecessors")
                for (auto candidateBB: predecessors(BBL)) {//TODO: optimize
                    if (sn_mapL.contains(candidateBB) && sn_mapL.lookup(candidateBB) == sn_mapR.lookup(BBRParent)) {
                        LOG("found BB candidate")
                        return {MatchType::blockSwitch, candidateBB};
                    }
                }
                ERR("could not find parent BB")
                assert(false);
            }
            return {blockSwitch, pred};
        } else
            return {allMatch};
    }

    void MergedBBComparator::merge(
            DenseMap<const BasicBlock *, std::pair<const BasicBlock *, DenseMap<const Value *, int>>> &BBMap,
            bool verbose
    ) {
        //TODO: look into mixing of submissions but this time we have BB knowledge
        this->verbose = verbose;

        LOG("==== merging ====")
        beginCompare(); //TODO: try with this commenting

        compareSignature(); // still need to call this because it updates sn_map

        SmallPtrSet<const BasicBlock *, 32> VisitedBBs; // in terms of F2.
        mergeBBPath = true;
        auto [res, newBB] = recurse(&FnL->getEntryBlock(), &FnR->getEntryBlock(), VisitedBBs, BBMap);
        outs() << "\n";
    }

    int MergedBBComparator::compare(
            DenseMap<const BasicBlock *, std::pair<const BasicBlock *, DenseMap<const Value *, int>>> &BBMap,
            bool verbose
    ) {
        this->verbose = verbose;

        LOG("==== cmp ====")
        beginCompare();

        if (int Res = compareSignature())
            return Res;

        SmallPtrSet<const BasicBlock *, 32> VisitedBBs; // in terms of F2.
        mergeBBPath = false;
        auto [res, newBB] = recurse(&FnL->getEntryBlock(), &FnR->getEntryBlock(), VisitedBBs, BBMap);
        outs() << "\n";
        return newBB.equiv; // 0 means match, 1 or -1 means not match
    }
} // merged