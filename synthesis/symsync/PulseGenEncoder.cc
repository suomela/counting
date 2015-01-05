//
// Symbolic SAT representation of "Synchronous Counting and Computational Algorithm Design"
//
// Siert Wieringa 2013-2014
//
#include"PulseGenEncoder.h"
using namespace SymSync;

// Constructor
PulseGenEncoder::PulseGenEncoder(int s, int n, int f, bool cyclic)
    : Encoder       (s, n, f, cyclic)
    , cexLength     (0)
    , findBaseStats (newSolverStats("Finding zero/one loop violations"))
    , findLoopStats (newSolverStats("Finding loops"))
    , findTransStats(newSolverStats("Finding a transition relation"))
    , findStabStats (newSolverStats("Finding non-stabilizing executions"))
{}

// Finds an abitrary transition relation satisfying the current set of constraints
TTable* PulseGenEncoder::findTransitionRelation () {
    if ( solve(findTransStats, zeroAt(0)) )
        return newTable(model);
    else
        return NULL;
}


// Finds a counterexample against a base property, assuming the last transition relation
// found by 'findTransitionRelation'. For Pulse Generation this means it finds an execution
// which is in state Zero at timepoint 0 and not in state One at timepoint 1, or the other
// way around.
bool PulseGenEncoder::findBasePropertyCex () {
    assert(nextTimeFrameId() >= 2);

    if ( solve(findBaseStats, zeroAt(0), ~oneAt(1)) ||
         solve(findBaseStats, oneAt(0), ~zeroAt(1)) ) {
        model.copyTo(cex);
        cexLength = 1;
        return true;
    }

    return false;
}


// Finds an execution of length 'k' in which literal 'l' is true,
// assuming the last transition relation found by 'findTransitionRelation'.
bool PulseGenEncoder::findLiteralTrue (StatsRef stats, int k, TTable* tt, Lit l) {
    assert(k < nextTimeFrameId());

    if ( tt ) {
        assumptions.capacity(tt->size()+1);
        tt->copyTo(assumptions);
    }
    else {
        assumptions.clear();
        assumptions.capacity(1);
    }
    assumptions.push_(l);
    budgetOff();

    if ( solve_(stats) == l_True ) {
        model.copyTo(cex);
        cexLength = k;
        return true;
    }

    return false;
}
