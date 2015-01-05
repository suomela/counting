//
// Symbolic SAT representation of "Synchronous Counting and Computational Algorithm Design"
//
// Siert Wieringa 2013-2014
//
#ifndef pulsegen_encoder_header
#define pulsegen_encoder_header
#include"Encoder.h"

namespace SymSync {

class PulseGenEncoder : public Encoder {
public:
    PulseGenEncoder(int s, int n, int f, bool cyclic);

    inline const char* encoderName          ()                          { return "PulseGen encoder"; }
    inline bool        findLocalPropertyCex (int k, TTable* tt = NULL)  { return findLiteralTrue(findLoopStats, k, tt, hasLoopTo(k)); }
    inline bool        findStabilizationCex (int k, TTable* tt = NULL)  { return findLiteralTrue(findStabStats, k, tt, ~stableAt(k)); }
    inline int         forbidLastCex        ()                          { assert(cexLength); return forbidExecution(cex, cexLength); }

    TTable* findTransitionRelation ();
    bool    findBasePropertyCex    ();
private:
    bool findLiteralTrue (StatsRef stats, int k, TTable* tt, Lit l);

    vec<lbool> cex;
    int      cexLength;

    const StatsRef findBaseStats;
    const StatsRef findLoopStats;
    const StatsRef findTransStats;
    const StatsRef findStabStats;
};

}

#endif
