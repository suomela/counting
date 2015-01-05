//
// Symbolic SAT representation of "Synchronous Counting and Computational Algorithm Design"
//
// Siert Wieringa 2013-2014
//
#include"Encoder.h"
#include"utils/System.h"
#include<cassert>
using namespace SymSync;

// Calculate integer representation of global state 'One'
static int calcOne(int s, int n) { int r = 0; for( int i = 0; i < n; i++ ) { r*= s; r++; } return r;}


// Constructor
Encoder::Encoder(int _s, int _n, int _f, bool _cyclic)
    : s             (_s)
    , n             (_n)
    , f             (_f)
    , cyclic        (_cyclic)
    , trueLit       (newLit()) // Variable 0
    , firstTableVar (nVars())  // Starting from variable 1...
    , lastTableVar  (firstTableVar + (cyclic?1:n)*s*s_pow(n))
    , One           (calcOne(s, n))
{
    // Variable 0 is forced to be always equal to true, for convenience.
    addClause(trueLit);

    // Encode one copy of the transition table
    encodeTransitionTable();

    // Encode one copy of the faulty node transition constraints
    encodeFaultyNodeSelection();
}


Encoder::~Encoder() {
    for( int i = 0; i < statsDB.size(); i++ )
        delete statsDB[i];
}


void Encoder::printStats(FILE *out) {
    fprintf(out, "Printing final statistics summary\n");
    fprintf(out, "\n*** Settings ***\n");
    fprintf(out, "Encoder name : %s\n", encoderName());
    fprintf(out, "-states      : %d\n", s);
    fprintf(out, "-nodes       : %d\n", n);
    fprintf(out, "-faulty      : %d\n", f);
    fprintf(out, "-cyclic      : %s\n", cyclic?"on":"off");

    fprintf(out, "\n*** Cumulative solver statistics ***\n");
#ifndef LINGELING
    fprintf(out, "nClauses()   : %-12d\n", nClauses());
    fprintf(out, "nVars()      : %-12d\n", nVars());
    fprintf(out, "nAssigns()   : %-12d\n", nAssigns());
    fprintf(out, "solves       : %-12"PRIu64"\n", solves);
    fprintf(out, "(re)starts   : %-12"PRIu64"\n", starts);
    fprintf(out, "conflicts    : %-12"PRIu64"\n", conflicts);
    fprintf(out, "decisions    : %-12"PRIu64"\n", decisions);
    fprintf(out, "propagations : %-12"PRIu64"\n", propagations);
#endif

#if defined(GLUCORED) || defined(MINIRED)
    fprintf(out, "\n*** solver/reducer statistics ***\n");
    fprintf(out, "workset inserts       : %-12"PRIu64"\n", workset_in);
    fprintf(out, "workset inserts lits  : %-12"PRIu64"\n", workset_in_lits);
    fprintf(out, "workset deletions     : %-12"PRIu64"\n", workset_deleted);
    fprintf(out, "workset deletions lits: %-12"PRIu64"\n", workset_deleted_lits);
    fprintf(out, "reducer inputs        : %-12"PRIu64"\n", reducer_in);
    fprintf(out, "reducer inputs lits   : %-12"PRIu64"\n", reducer_in_lits);
    fprintf(out, "reducer outputs       : %-12"PRIu64"\n", reducer_out);
    fprintf(out, "reducer outputs lits  : %-12"PRIu64"\n", reducer_out_lits);
    fprintf(out, "reducer not outp. lits: %-12"PRIu64"\n", reducer_notout_lits);
    fprintf(out, "reducer forced backtr.: %-12"PRIu64"   (avg. %.1f levels from level %.1f)\n", reducer_backtracks, reducer_backtracks?((float)reducer_backtrack_levels / (float)reducer_backtracks):0, reducer_backtracks?(reducer_backtrack_level_before / (float)reducer_backtracks):0);
    fprintf(out, "reducer f.backtr. to 0: %-12"PRIu64"\n\n", reducer_backtracks_tozero);
#endif

    fputc('\n', out);
    for( int i = 0; i < statsDB.size(); i++ )
        statsDB[i]->print(out);
}



// Encode selection of faulty nodes
void Encoder::encodeFaultyNodeSelection() {
    // Handle special case where no node is faulty
    if ( f == 0 ) {
        faulty.growTo(n, ~trueLit);
        return;
    }
    // In case of f=1 and cyclic algorithm we may assume that node 0 is the faulty node
    else if ( f == 1 && cyclic ) {
        faulty.growTo(n, ~trueLit);
        faulty[0] = trueLit;
        return;
    }

    vec<vec<Lit> > feq;  // The truth of variable 'feq[i][k]' represents that node 'i' is the 'k'th faulty node.
    vec<vec<Lit> > fleq; // The truth of variable 'fleq[i][k]' represents that the index of the 'k'th faulty node is <= 'i'.

    faulty.capacity(n);
    for( int i = 0; i < n; i++ ) {
        feq.push();
        fleq.push();
        vec<Lit>& feqi  = feq.last();
        vec<Lit>& fleqi = fleq.last();

        faulty.push_(newLit());
        vec<Lit> cstr9; // For constraint (F9)
        cstr9.capacity(f+1);
        cstr9.push_(~faulty.last());

        feqi.capacity(f);
        fleqi.capacity(f);
        for( int k = 0; k < f; k++ ) {
            feqi.push_(newLit());
            fleqi.push_(newLit());

            cstr9.push_(feqi[k]);
            addClause(~feqi[k], fleqi[k]);        // Constraint (F1) : If the 'k'th faulty node has id 'i' then it has id <= 'i'
            addClause(~feqi[k], faulty.last());   // Constraint (F8) : If node 'i' is the 'k'th faulty node then node 'i' is a faulty node
            if ( i > 0 ) {
                addClause(~fleqi[k], feqi[k], fleq[i-1][k]); // Constraint (F2) : For i>0, if the 'k'th faulty node has id <= 'i' then it has id 'i' or id < 'i'
                addClause(~fleq[i-1][k], fleqi[k]);          // Constraint (F3) : For i>0, if the 'k'th faulty node has id < 'i' then it has id <= 'i'
                addClause(~fleq[i-1][k], ~feqi[k]);          // Constraint (F4) : For i>0, if the 'k'th faulty node has id < 'i' then it does not have id 'i'
            }

            if ( k > 0 )
                addClause(~feqi[k-1], ~fleqi[k]); // Constraint (F7): For k>0, if the 'k-1'th faulty node has id 'i' then the 'k'th faulty node does not have id <= 'i' (forces unique ordering)
        }

        addClause_(cstr9); // Constraint (F9): If node 'i' is faulty then it must be chosen as the 'k'th faulty node for some value of 'k'.
    }

    for( int k = 0; k < f; k++ ) {
        addClause(feq[0][k], ~fleq[0][k]); // Constraint (F5) : If the 'k'th faulty node does not have id 0 it does not have id <= 0
        addClause(fleq.last()[k]);         // Constraint (F6) : The 'k'th faulty node has id < n ("there is a 'k'th faulty node").
    }
}


// Encode the transition table
void Encoder::encodeTransitionTable () {
    assert(firstTableVar == nVars());
    while( nVars() < lastTableVar ) newVar();

    // 'exactly one successor' constraint on the table entries for all global states 'g' and nodes 'i'
    vec<Lit> cstr;
    cstr.capacity(s);
    for( int i = 0; i < n; i++ ) {
        for( int g = 0; g < s_pow(n); g++ ) {
            cstr.clear();

            for( int p = 0; p < s; p++ ) {
                cstr.push_(tableLit(g, i, p));
                for( int q = 0; q < p; q++ )
                    addClause(~tableLit(g, i, p), ~tableLit(g, i, q)); // Constraint T1: Successor of g,i=p implies successor of g,i!=q
            }
            addClause_(cstr); // Constraint T2: g,i has a successor
        }

        if ( cyclic ) break; // There is no need to repeat this for all 'i' in case we're dealing with cyclic algorithms
    }
}


// Create a complete new timeframe, return its index (starting from zero)
int Encoder::encodeNextTimeFrame() {
    const int k = encodeNextState();

    encodeTransitionRelation(k);
    encodeZeroOne(k);
    encodeHasLoop(k);

    return k;
}


// Encode the next time frame
int Encoder::encodeNextState() {
    const int k = nextTimeFrameId();

#ifdef GITHUB_MINISAT
    vec<Var> freeTmp; // The variables created here must have consecutive indices, so temporary disable return of previously released variables from 'newVar'
    free_vars.copyTo(freeTmp);
    free_vars.clear();
#endif

    timeFrameOffset.push(newVar()); // Keep track of the first variable of this frame
    if ( cyclic && f == 1 ) {
        // Create (2n-1) * s variables
        for( int i = 1; i < (2*n-1) * s; i++ ) newVar();
    }
    else {
        // Create n^2 * s variables
        for( int i = 1; i < n * n * s; i++ ) newVar();

        for( int i = 0; i < n; i++ )
            for( int j = 0; j < n; j++ )
                for( int p = 0; p < s; p++ )
                    if ( i != j ) addClause(faulty[i], ~globalLit(k,i,p), observedLit(k,i,j,p)); //  Constraint O1: if node i is not faulty then the state of node 'i' as observed by node 'j' is equal to the actual state of node 'j'
    }

#ifdef GITHUB_MINISAT
    freeTmp.copyTo(free_vars);
#endif

    // 'exactly one successor' constraint on the table entries for all global states 'g' and nodes 'i'
    vec<Lit> cstr;
    cstr.capacity(s);
    for( int i = 0; i < n; i++ ) {
        for( int j = 0; j < n; j++ ) {
            cstr.clear();
            for( int p = 0; p < s; p++ ) {
                cstr.push_(observedLit(k, i, j, p));

                for( int q = 0; q < p; q++ )
                    addClause(~observedLit(k, i, j, p), ~observedLit(k, i, j, q)); // Constraint O2: If node 'j' observes state of node 'i' as 'p' then it does not observe 'q'
            }
            addClause_(cstr); // Constraint O3: Node 'j' observes some state of node 'i'

            if ( cyclic && f==1 && i>0 ) break;
        }
    }

    return k;
}



// Encode 'Zero(k)', 'One(k)' and 'stableAt(k)'.
void Encoder::encodeZeroOne(const int k) {
    assert(k+1 == timeFrameOffset.size());
    assert(zero.size() == k);
    assert(one.size()  == k);

    // Create a variable denoting we're in global state zero, and one for global state one
    zero.push(newLit());
    one.push (newLit());
    stable.push (newLit());

    addClause(~zero[k], stable[k]);         // Constraint Z11
    addClause(~one[k], stable[k]);          // Constraint Z12
    addClause(~stable[k], zero[k], one[k]); // Constraint Z13

    vec<Lit> zki, oki;
    zki.capacity(n);
    oki.capacity(n);

    // Create vectors for constraints Z3 and Z4
    vec<Lit> cstrZ3, cstrZ4;
    cstrZ3.capacity(n+1);
    cstrZ3.push_(zero[k]);
    cstrZ4.capacity(n+1);
    cstrZ4.push_(one[k]);

    // Constraints Z1 and Z2 + create variables 'zki' and 'oki' for all 'i'
    for( int i = 0; i < n; i++ ) {
        zki.push_(newLit());
        addClause(~zero[k], zki[i]); // Constraint Z1

        oki.push_(newLit());
        addClause(~one[k], oki[i]);  // Constraint Z2

        cstrZ3.push_(~zki[i]);
        cstrZ4.push_(~oki[i]);
    }

    // Add constraints Z3 and Z4
    addClause_(cstrZ3);
    addClause_(cstrZ4);

    for( int i = 0; i < n; i++ ) {
        addClause(~zki[i], faulty[i], globalLit(k,i,0)); // Constraint Z5
        addClause(~oki[i], faulty[i], globalLit(k,i,1)); // Constraint Z6
        addClause(~globalLit(k,i,0), zki[i]);            // Constraint Z7
        addClause(~globalLit(k,i,1), oki[i]);            // Constraint Z8
        addClause(~faulty[i], zki[i]);                   // Constraint Z9
        addClause(~faulty[i], oki[i]);                   // Constraint Z10
    }
}


// Enforce the transition relation at timepoint 'k'
void Encoder::encodeTransitionRelation(const int k) {
    if ( !k ) return;

    vec<Lit> cstr;
    cstr.capacity(n+2);
    for( int g = 0; g < s_pow(n); g++ ) {
        for( int j = 0; j < n; j++ ) { // For all observing nodes
            cstr.clear();
            cstr.push_(lit_Undef);
            cstr.push_(lit_Undef);
            for( int i = 0; i < n; i++ ) // For all actual nodes
                cstr.push_(~observedLit(k-1, i, j, stateOf(g, i)));

            // cstr now includes partial constraint 'g ->'
            for( int p = 0; p < s; p++ ) {
                cstr[0] = ~globalLit(k, j, p);
                cstr[1] = tableLit(g, j, p);
                addClause(cstr);
            }
        }
    }
}


// Encode the 'hasLoop(k)' constraint
void Encoder::encodeHasLoop(const int k) {
    assert(k == hasLoop.size());
    if ( k == 0 ) { hasLoop.push(~trueLit); return; }

    hasLoop.push(newLit());
    addClause(~hasLoop[k], ~stableAt(k)); // Constraint L1

    vec<Lit> cstr;
    cstr.growTo(4, ~hasLoop[k]);
    for( int i = 0; i < n; i++ ) {
        assert(cstr[0] == ~hasLoop[k]);
        cstr[1] = isFaulty(i);
        for( int p = 0; p < s; p++ ) {
            cstr[2] = ~globalLit(0, i, p);
            cstr[3] =  globalLit(k, i, p);
            addClause(cstr); // Constraint L2
        }
    }
    assert(value(hasLoop[k]) == l_Undef);
}


// Return the global state at timepoint 'k' in the execution represented by 'm'. Returns the state of faulty nodes as 0.
int Encoder::globalState(const vec<lbool>& m, int k) const {
    int g = 0;

    for (int i = 0; i < n; i++ ) {
        if ( isFalse (isFaulty(i), m) ) {
            for( int p = 0; p < s; p++ )
                if ( isTrue(globalVar(k, i, p), m) ) { g+=p; break; }
        }
        g = ror(g);
    }

    return g;
}


// Return the global state at timepoint 'k' as it was observed by non-faulty node 'j'
int Encoder::observedGlobalState(const vec<lbool>& m, int j, int k) const {
    int g = 0;
    assert(isFalse(isFaulty(j), m));

    for (int i = 0; i < n; i++ )
        for( int p = 0; p < s; p++ )
            if ( isTrue(observedVar(k, i, j, p), m) ) { g+=p; g = ror(g); break; }

    return g;
}


#ifdef FORBID_COMPLEX
// Increment 'gOb' to the next possible permutation ("change the lies of the faulty nodes")
// Returns a negative value when it reaches the last permutation
void Encoder::nextObservablePermutation(int fId, int& gOb) {
    if ( !cyclic || f!=1 ) {
        int i;
        for( i = 0; i < n; i++ )
            if ( (fId & (1<<i)) && incStateOf(gOb, i) ) break;

        if ( i == n ) gOb = -1;
    }
    else if ( !(++gOb % s) ) gOb=-1;
}


// Given the representation of the faulty nodes 'fId', return a literal which, when forced to true,
// enforces the constraint that the next state of global state 'g' in node 'i' can not be 'p'.
Lit Encoder::forbidNextState(int fId, int g, int i, int p) {
    assert(!(fId&(1<<i))); // 'i' should not itself be a faulty node

    // TODO Implement this using a more sensible data structure
    forbidState.growTo(fId+1);
    forbidState[fId].growTo(g+1);
    forbidState[fId][g].growTo(i+1);
    forbidState[fId][g][i].growTo(p+1, lit_Undef);

    Lit& ret = forbidState[fId][g][i][p];
    if ( ret != lit_Undef ) return ret;

    // Check the value of the table literals that will be involved in the binary clauses we want to create
    int cnt = 0;
    for( int gOb = g; gOb >= g; nextObservablePermutation(fId, gOb)) {
        lbool v = value(tableLit(gOb, i, p));
        if ( v == l_True )       { ret = ~trueLit; return ret; }        // Node 'i' will always have 'p' as a possible next state of 'g'
        else if ( v == l_Undef ) { cnt++; ret = ~tableLit(gOb, i, p); }
    }

    if ( cnt == 0 )
        ret = trueLit;
    else if ( cnt > 1 ) {
        ret = newLit();

        // Create the binary clauses
        for( int gOb = g; gOb >= g; nextObservablePermutation(fId, gOb))
            addClause(~ret, ~tableLit(gOb, i, p));
    }

    return ret;
}
#endif

// Create a new constraint that forbids the first 'k' execution steps of the execution represented by 'm'
int Encoder::forbidExecution(const vec<lbool>& m, int k) {
    vec<Lit> cstr;
    cstr.capacity(k*(n-f));

#ifdef FORBID_COMPLEX
    // Integer identifying the faulty nodes in execution 'm'
    int fId = 0;
    for( int i = 0; i < n; i++ )
        if ( isTrue(isFaulty(i), m) ) fId+= (1<<i);
#endif

    // For every time point
    for( int t = 0; t < k; t++ ) {
        const int g = globalState(m, t);
        // For every non-faulty node
        for( int j = 0; j < n; j++ ) {
            if ( isTrue(isFaulty(j), m) ) continue;

            const int gOb = observedGlobalState(m, j, t); // The global state 'gOb' as node 'j' observed it (equal to 'g' for all non-faulty nodes)
            for( int p = 0; p < s; p++ )
                if ( isTrue(tableVar(gOb,j,p), m) ) {
#ifdef FORBID_COMPLEX
                    // This is the more complex version, no observable permutation of 'g' can have next state 'p' for node 'j' if 'forbidNextState(fId, g, j, p)' is true.
                    cstr.push( forbidNextState(fId, g, j, p) );
#else
                    // The simple version. The next state of the permutation 'gOb' that node 'j' observed in the execution represented by 'm' should no longer equal 'p'
                    cstr.push(~tableLit(gOb, j, p));
#endif
                    break; }
        }
    }
    assert(cstr.size() == (k)*(n-f));
    addClause_(cstr);

    return cstr.size(); // This will return the length of the clause after removing duplicate literals and falsified literals.
}


// Print the transition table in 'tt' to the file pointer 'out'
void Encoder::printTransitionTable(FILE *out, TTable* tt) {
    assert(tt);

    for( int g = 0; g < s_pow(n); g++ ) {
        for( int i = 0; i < n; i++ )
            fprintf(out, "%s%d", (s>10)?"s":"", stateOf(g, i));

        fputc(' ', out);
        for( int i = 0; i < n; i++ ) {
            int cnt = 0;
            for( int k = 0; k < s; k++ )
                if ( isTrue(tableVar(g,i,k), tt->model) ) { cnt++; fprintf(out, "%s%d", (s>10)?"s":"", k); }
            assert(cnt==1);
        }
        fputc('\n', out);
    }
}


// Specify a row in the table completely
bool Encoder::enforceTransition(int gFrom, int gTo) {
    for( int i = 0; i < n; i++ )
        addClause(tableLit(gFrom, i, stateOf(gTo, i)));

    return okay();
}


// For debugging
void Encoder::printExecution(FILE *out, const vec<lbool>& m, int k) {
    for( int t = 0; t <= k; t++ ) {
        int g = globalState(m, t);
        for( int i = 0; i < n; i++ ) {
            if ( isTrue(isFaulty(i), m) )
                fprintf(out, "f");
            else
                fprintf(out, "%d", stateOf(g, i));
        }
        fprintf(out, "\n");
    }
}


// Create a new transition table from the specified model 'm'
TTable* Encoder::newTable(const vec<lbool>& m) {
    TTable* tt = new TTable(m);

#ifdef COMPACT_ASSUMPTIONS
    Lit l = newLit();
    tt->push(~l);
    for( Var v = firstTableVar; v < lastTableVar; v++ )
        addClause(l, mkLit(v, !isTrue(v, m)));
#else
    tt->capacity(lastTableVar - firstTableVar);
    for( Var v = firstTableVar; v < lastTableVar; v++ )
        tt->push_(mkLit(v, !isTrue(v, model)));
#endif

    return tt;
}

// Delete the specified transition table
void Encoder::deleteTable(TTable* tt) {
    assert(tt);

#ifdef COMPACT_ASSUMPTIONS
    assert(tt->size() <= 1);
    if ( tt->size() ) addClause(~tt->last());
#endif
    delete tt;
}


// Constructor for solver statistics
Encoder::Stats::Stats (const char* descr, const Encoder& _parent)
    : description (strdup(descr))
    , parent      (_parent)
    , solves      (0)
    , starts      (0)
    , conflicts   (0)
    , decisions   (0)
    , propagations(0)
    , time        (0)
{}


// Destructor for solver statistics
Encoder::Stats::~Stats() {
    free(description);
}


// Printing routine for solver statistics
void Encoder::Stats::print(FILE *out) {
    fprintf(out, "*** Statistics for solver usage '%s' ***\n", description);
#ifndef LINGELING
    fprintf(out, "calls        : %-12"PRIu64"\n", solves);
    if ( solves ) {
        fprintf(out, "(re)starts   : %-12"PRIu64" (%.1f per call)\n", starts, starts/(float) solves);
        fprintf(out, "conflicts    : %-12"PRIu64" (%.1f per call)\n", conflicts, conflicts/(float) solves);
        fprintf(out, "decisions    : %-12"PRIu64"\n", decisions);
        fprintf(out, "propagations : %-12"PRIu64"\n", propagations);
    }
#endif
    fprintf(out, "time         : %.2f s\n\n", time);
}


// Start execution, record current state of solver, and current CPU time
void Encoder::Stats::start() {
#ifndef LINGELING
    s = parent.solves;
    r = parent.starts;
    c = parent.conflicts;
    d = parent.decisions;
    p = parent.propagations;
#endif
    t = cpuTime();
}


// End of execution, add differences since 'start()' to the statistics.
void Encoder::Stats::stop() {
    time        += cpuTime() - t;
#ifndef LINGELING
    solves      += parent.solves - s;
    starts      += parent.starts - r;
    conflicts   += parent.conflicts - c;
    decisions   += parent.decisions - d;
    propagations+= parent.propagations - p;
#endif
}
