//
// Symbolic SAT representation of "Synchronous Counting and Computational Algorithm Design"
//
// Siert Wieringa 2014
//
#ifndef lingeling_wrapper_header
#define lingeling_wrapper_header
#include"core/Solver.h"
extern "C" {
#include"lglib.h"
};

#define toLgLit(l) ((var((l))+1)*(sign((l))?-1:1))

using namespace Minisat;

namespace SymSync {

class LingelingWrapper {
public:
 LingelingWrapper() : lgl(lglinit()) { lglsetopt(lgl, "bve", 0); }
    ~LingelingWrapper() { lglrelease(lgl); }

    inline Var  newVar()      { lglincvar(lgl); Var v = lglmaxvar(lgl); lglfreeze(lgl, v); return v-1; }
    inline int  nVars ()      { return lglmaxvar(lgl); }
    inline bool okay  ()      { return !lglinconsistent(lgl); }

    inline bool addClause (Lit p)               { lgladd(lgl,toLgLit(p)); lgladd(lgl,0); return okay(); }
    inline bool addClause (Lit p, Lit q)        { lgladd(lgl,toLgLit(p)); lgladd(lgl,toLgLit(q)); lgladd(lgl,0); return okay(); }
    inline bool addClause (Lit p, Lit q, Lit r) { lgladd(lgl,toLgLit(p)); lgladd(lgl,toLgLit(q)); lgladd(lgl,toLgLit(r)); lgladd(lgl,0); return okay(); }
    inline bool addClause_(const vec<Lit>& ps)  { return addClause(ps); }

    bool addClause (const vec<Lit>& ps)  { for( int i = 0; i < ps.size(); i++ ) lgladd(lgl, toLgLit(ps[i])); lgladd(lgl, 0); return okay(); }


    inline lbool value(Var v) { int r = lglfixed(lgl, v+1); return ((r==0)?l_Undef:(r<0 ? l_False: l_True)); }
    inline lbool value(Lit l) { lbool r = value(var(l)); return r ^ sign(l); }
    inline void  melt (Var v) { lglmelt(lgl, v+1); }

    inline bool solve(Lit p)                   { assumptions.clear(); assumptions.push(p); return solve_() == l_True; }
    inline bool solve(Lit p, Lit q)            { assumptions.clear(); assumptions.push(p); assumptions.push(q); return solve_() == l_True; }
    inline bool solve(Lit p, Lit q, Lit r)     { assumptions.clear(); assumptions.push(p); assumptions.push(q); assumptions.push(r); return solve_() == l_True; }
    inline bool solve(const vec<Lit>& assumps) { assumps.copyTo(assumptions); return solve_() == l_True; }

    lbool solve_() {
	for( int i = 0; i < assumptions.size(); i++ ) 
	    lglassume(lgl, toLgLit(assumptions[i]));

	int r = lglsat(lgl);
	if ( r == LGL_SATISFIABLE ) {
	    model.clear();
	    model.capacity(nVars());
	    for( Var v = 1; v <= nVars(); v++ ) {
		int d = lglderef(lgl, v);
		model.push_((d>0) ? l_True: ((d<0) ? l_False: l_Undef));
	    }
	}

	return (r == LGL_SATISFIABLE) ? l_True : ((r==LGL_UNSATISFIABLE)? l_False: l_Undef);
    }

    inline void budgetOff () {};

    vec<lbool> model;
    vec<Lit>   assumptions;
private:
    LGL* const lgl;
};

};

#endif
