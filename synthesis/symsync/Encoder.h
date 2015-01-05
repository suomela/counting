//
// Symbolic SAT representation of "Synchronous Counting and Computational Algorithm Design"
//
// Siert Wieringa 2013
//
#ifndef encoder_header
#define encoder_header
#include<cstdio>
#include<cmath>
#ifdef LINGELING
#include"LingelingWrapper.h"
#elif defined(GLUCORED) || defined(MINIRED)
#include"solver-reducer/SolRed.h"
#else
#include"core/Solver.h"
#endif

#if defined(GLUCORED)
using namespace GlucoRed;
#elif defined(MINIRED)
using namespace MiniRed;
#else
using namespace Minisat;
#endif

namespace SymSync {

// A transition table is a set of assumptions, and a set of lifted Boolean's.
// Instances of this class must be created using 'Encoder::newTable' and destructed using 'Encoder::deleteTable'
class TTable : public vec<Lit> {
private:
    friend class Encoder;
    TTable(const vec<lbool>& m) { m.copyTo(model); }
    vec<lbool> model;
};

#ifdef LINGELING
typedef LingelingWrapper EncoderBase;
#elif defined(GLUCORED) || defined(MINIRED)
typedef SolRed EncoderBase;
#else
typedef Solver EncoderBase;
#endif

class Encoder: protected EncoderBase {
public:
    // Constructor
    Encoder(int _s, int _n, int _f, bool _cyclic);
    ~Encoder();

    // Encode the next time frame, return frame index (starting from zero)
    virtual int encodeNextTimeFrame();

    // The global state 'gFrom' must result in the global state 'gTo'
    bool enforceTransition(int gFrom, int gTo);

    virtual const char* encoderName () = 0;
    virtual bool    findLocalPropertyCex   (int k, TTable* tt = NULL) = 0;
    virtual bool    findStabilizationCex   (int k, TTable* tt = NULL) = 0;
    virtual TTable* findTransitionRelation () = 0;
    virtual bool    findBasePropertyCex    () = 0;
    virtual int     forbidLastCex () = 0;

    // Print the transition table represented by the model in 'm'
    void printTransitionTable(FILE *out, TTable *tt);

    // Return the total number of timeframes encode (=frame index of next time frame)
    inline int nextTimeFrameId() const { return timeFrameOffset.size(); }

    // These are public but can't be modified as they're constants.
    //
    const int  s;         // Number of states 's'
    const int  n;         // Number of nodes 'n'
    const int  f;         // Number of faulty nodes 'f'
    const bool cyclic;    // Restrict to cyclic algorithms only
    const Lit  trueLit;       // A literal forced to true in the solver
    const Var  firstTableVar; // The first variable that is in the transition table
    const Var  lastTableVar;  // Number of variables in the transition table

    void deleteTable (TTable* tt);
    void printStats  (FILE* stats);
protected:
    // Solver statistics
    class Stats {
    public:
        Stats (const char* descr, const Encoder& _parent);
        ~Stats();
        void start();
        void stop ();
        void print(FILE *out);
    private:
        char* description;
        const Encoder& parent;
        uint64_t s, solves;
        uint64_t r, starts;
        uint64_t c, conflicts;
        uint64_t d, decisions;
        uint64_t p, propagations;
        double   t, time;
    };
    typedef int StatsRef; // Currently this is an integer denoting the index in 'statsDB', it can be easily changed to for example 'Stats*', if necessary.

    inline StatsRef newSolverStats(const char* descr) { const StatsRef ret = statsDB.size(); statsDB.push(new Stats(descr, *this)); return ret; }
    inline bool     solve  (StatsRef stats)                      { budgetOff(); assumptions.clear(); return solve_(stats) == l_True; }
    inline bool     solve  (StatsRef stats, Lit p)               { budgetOff(); assumptions.clear(); assumptions.push(p); return solve_(stats) == l_True; }
    inline bool     solve  (StatsRef stats, Lit p, Lit q)        { budgetOff(); assumptions.clear(); assumptions.push(p); assumptions.push(q); return solve_(stats) == l_True; }
    inline bool     solve  (StatsRef stats, Lit p, Lit q, Lit r) { budgetOff(); assumptions.clear(); assumptions.push(p); assumptions.push(q); assumptions.push(r); return solve_(stats) == l_True; }

    inline lbool solve_ (StatsRef stats) { 
	// Solver hacks can be placed here, for example:
	//if ( nLearnts() > 100000 ) reduceDB();

	statsDB[stats]->start();
	const lbool ret = EncoderBase::solve_();
	statsDB[stats]->stop();

	return ret; 
    }

    TTable* newTable   (const vec<lbool>& m);

    // Add a constraint which forces a change in the transition relation that forbids the first 'k' steps of the
    // execution in 'm'
    int forbidExecution(const vec<lbool>& m, int k);

    // For convience
    static inline bool isFalse(Var v, const vec<lbool>& m) { return m[v] == l_False; }
    static inline bool isFalse(Lit l, const vec<lbool>& m) { return m[var(l)] == (sign(l) ? l_True: l_False);}
    static inline bool isTrue (Var v, const vec<lbool>& m) { return m[v] == l_True; }
    static inline bool isTrue (Lit l, const vec<lbool>& m) { return m[var(l)] == (sign(l) ? l_False: l_True); }
    static inline bool isUndef(Var v, const vec<lbool>& m) { return m[v] == l_Undef; }
    static inline bool isUndef(Lit l, const vec<lbool>& m) { return m[var(l)] == l_Undef; }

    // Index the literals with special semantics
    inline Lit zeroAt     (int k) const { assert(k<zero.size());    return zero[k]; }
    inline Lit oneAt      (int k) const { assert(k<one.size());     return one[k]; }
    inline Lit stableAt   (int k) const { assert(k<stable.size());  return stable[k]; }
    inline Lit hasLoopTo  (int k) const { assert(k<hasLoop.size()); return hasLoop[k]; }
    inline Lit isFaulty   (int i) const { assert(i<faulty.size());  return faulty[i]; }

    // Create new variable and return as positive literal
    inline Lit newLit () { return mkLit(newVar(), false); }

    // For debugging
    void printExecution(FILE *out, const vec<lbool>& m, int k);

    // State of node 'i' in global state 'g'
    inline int stateOf    (int g, int i)  const { g/= s_pow(i); /* divide by s^i */ return g%s; /* return result modulo s */ }
    // Increment state of node 'i' in global state 'g' by one (modulo 's')
    inline int incStateOf (int& g, int i) const { const int t = (stateOf(g,i) + 1)%s; if ( t ) g+= s_pow(i); else g-= (s-1) * s_pow(i); return t; }

    // Return 's^i'. Would it be better to implement this with a lookup table?
    inline int s_pow  (int i)         const { return (int) pow(s,i); }
    // Rotate global state 'g' right by 1 state
    inline int ror    (int g)         const { int r = g%s; g+=r*s_pow(n); g/=s; return g; }
    // Rotate global state 'g' right by 'i' states
    inline int rotate (int g, int i)  const { for( int j = 0; j < i; j++ ) g=ror(g); return g; }

    // For index the transition table
    inline Var tableVar(int g, int i, int p)           const { return cyclic ? (firstTableVar + (rotate(g,i)*s) + p): (firstTableVar + (g*n*s) + (i*s) + p); }
    inline Lit tableLit(int g, int i, int p)           const { return mkLit(tableVar(g, i, p), false); }

    // For indexing bits of the observed and actual global state
    inline Var observedVar(int k, int i, int j, int p) const { if ( i == 0 || !cyclic || f!=1 ) return timeFrameOffset[k] + (i*n*s) + (j*s) + p;
        else return timeFrameOffset[k] + (n*s) + ((i-1)*s) + p; }
    inline Var globalVar(int k, int i, int p)          const { return observedVar(k, i, i, p); }
    inline Lit observedLit(int k, int i, int j, int p) const { return mkLit(observedVar(k,i,j,p), false); }
    inline Lit globalLit(int k, int i, int p)          const { return mkLit(globalVar(k,i,p), false); }

    void encodeFaultyNodeSelection ();

#ifdef FORBID_COMPLEX
    void nextObservablePermutation (int fId, int& gOb);
    // The next state of node 'i' given global state 'g' (or any observable permutation of it) may not be 'p' (forced by assigning returned literal to true)
    Lit forbidNextState(int fId, int g, int i, int p);
    vec<vec<vec<vec<Lit> > > > forbidState; // TODO : A more sensible implementation
#endif

    // Returns the global state of the system as observed by node 'j' at timepoint 'k'
    int  observedGlobalState       (const vec<lbool>& m, int j, int k) const;
    // Returns the global state of the system at timepoint 'k', the state of all faulty nodes is assumed to be '0'
    int  globalState               (const vec<lbool>& m, int k)        const;
    int  encodeNextState           ();
    void encodeZeroOne             (const int k);
    void encodeTransitionRelation  (const int k);
    void encodeHasLoop             (const int k);
    void encodeTransitionTable     ();

    const int One;

    vec<Lit> zero, one, stable, hasLoop;
    vec<Var> timeFrameOffset;
    vec<Lit> faulty; // faulty[i] can be true only if 'i' is not chosen as a faulty node.

private:
    vec<Stats*> statsDB;
};

};

#endif

