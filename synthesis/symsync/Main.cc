//
// Symbolic SAT representation of "Synchronous Counting and Computational Algorithm Design"
//
// Siert Wieringa 2013-2014
//
#include<cstdio>
#include<cstdlib>
#include<signal.h>
#include"core/Solver.h"
#include"PulseGenEncoder.h"
#include"utils/Options.h"
#include"utils/System.h"

#define SYMSYNC "SymSync"

#define RET_FAIL 1
#define RET_SAT 2
#define RET_UNSAT 3

static IntOption    opt_states  (SYMSYNC, "states", "The number of states a node can be in.", 3, IntRange(1, INT32_MAX));
static IntOption    opt_nodes   (SYMSYNC, "nodes" , "The number of nodes.", 4, IntRange(1, INT32_MAX));
static IntOption    opt_faulty  (SYMSYNC, "faulty", "The number of faulty nodes.", 1, IntRange(0, INT32_MAX));
static IntOption    opt_time    (SYMSYNC, "time"  , "Target stabilization time (-1=infinity).", -1, IntRange(-1, INT32_MAX));
static BoolOption   opt_cyclic  (SYMSYNC, "cyclic", "Restrict search to cyclic algorithms.", true);

static BoolOption   opt_verify   (SYMSYNC, "verify", "Verify that the given file contains the transition table of a stabilizing algorithm.", false);
static IntOption    opt_overshoot(SYMSYNC, "overshoot", "Overshoot the target stabilization time by at most this many timepoints (-1=infinity).", 0, IntRange(-1, INT32_MAX));
static BoolOption   opt_min      (SYMSYNC, "min", "Aim to find the minimal stabilizing algorithm (equivalent to setting time=0 and overshoot=-1)", false);

static StringOption opt_algdir(SYMSYNC, "algdir", "Directory into which generated algorithm files are stored", "");

using namespace SymSync;

static bool     tableLoaded;
static Encoder* e;

// Print the current CPU time on a new line
static void printTime(FILE *out) {
    fprintf(out, "\n[%.2f s] ", cpuTime());
}


// Print statics
void printStats(FILE* fp) {
    if ( !e ) return;

    printTime(fp);
    e->printStats(fp);
}


// Signal handler, print statistics and exit
static void SIGINT_exit(int) {
    fprintf(stderr, "\n\nInterrupted.");
    printStats(stderr);
    exit(1);
}


// Read one state vector from a file
int readVector(FILE *fp, bool allowEOF) {
    if ( e->s > 10 ) {
        fprintf(stderr, "Reading transition tables from file is not implemented for s>10\n"); // TODO
        return false;
    }

    int G = 0;
    for( int i = 0; i < e->n; i++ ) {
        int c = fgetc(fp);

        if ( i == 0 && allowEOF && c == EOF ) {
            return 0;
        }
        else if ( c == EOF ) {
            fprintf(stderr, "Unexpected end-of-file\n");
            return -1;
        }
        else if ( c == ' ' ) {
            fprintf(stderr, "Expected input file for '%d' nodes, read input vector of length '%d'\n", e->n, i);
            return -1;
        }
        else if ( c < '0' || c > '9' ) {
            fprintf(stderr, "Unexpected character '%c' in input stream, expected integer 0-%d\n", (char) c, e->s-1);
            return -1;
        }
        else if ( c - '0' > e->s ) {
            fprintf(stderr, "Expected input file for '%d' states (0-%d), input vector contains state '%c'\n", e->s, e->s-1, (char) c);
            return -1;
        }

        G=G*e->s+(c-'0');
    }

    return G;
}


// Enforce the transition table found in the file
bool enforceTransitionTable(FILE *fp) {
    assert(fp);
    assert(e->s<=10);

    while(true) {
        // Read state vector
        int in = readVector(fp, true);
        if ( in < 0 ) return false;
        else if ( feof(fp) ) return true;

        // Read space
        int c = fgetc(fp);
        if ( c == EOF ) {
            fprintf(stderr, "Expected space, got end-of-file\n");
            return false;
        }
        else if ( c != ' ' ) {
            fprintf(stderr, "Expected space, got '%c'\n", (char) c);
            return false;
        }

        // Read next state vector
        int out = readVector(fp, false);
        if ( out < 0 ) return false;

        // Read line end
        c = fgetc(fp);
        if ( c == EOF ) return true;
        else if ( c != '\n' ) {
            fprintf(stderr, "Expected end-of-line, got '%c'\n", (char) c);
            return false;
        }

        tableLoaded = true;
        if ( !e->enforceTransition(in, out) ) {
            fprintf(stderr, "The input transition relation contains contradictory entries%s.\n", e->cyclic?", or it is not cylic":"");
            return false;
        }
    }
}


// Print an algorithm to file
void printResult(int k, TTable* tt) {
    const int buflen = 1024;
    char filename[buflen];
    const char *path = opt_algdir;
    snprintf(filename, buflen, "%salg-%d-%d-%d-%d%s.txt", path, e->s, e->n, e->f, k, e->cyclic?"-c":"");

    printTime(stderr);
    fprintf(stderr, "Printing transition table which will ensure stabilization after %d timesteps to file '%s'.", k, filename);

    FILE*out = fopen(filename, "wt");
    if ( out ) {
        e->printTransitionTable(out, tt);
        fclose(out);
    }
    else {
        fprintf(stderr, "ERROR: Failed to open file '%s' for writing. Will write to 'stdout' instead.\n", filename);
        e->printTransitionTable(stdout, tt);
    }
}


// Verify an algorithm loaded from file
int verify(int time) {
    if ( e->nextTimeFrameId() != 0 ) return 1; // Expects a "clean" Encoder

    fprintf(stderr, "Will verify stabilization in -time=");
    if ( time < 0 ) fprintf(stderr, "infinity"); else fprintf(stderr, "%d", time);
    fprintf(stderr, ".\n");

    int k = 0;
    while( k < 1 )
        k = e->encodeNextTimeFrame();

    printTime(stderr);
    fprintf(stderr, "Checking base property... ");
    if ( e->findBasePropertyCex() ) {
        fprintf(stderr, "failed");
        return 1;
    }
    fprintf(stderr, "ok");

    // Unroll up to 'time', does nothing if 'time' is negative (infinity)
    while( k < time )
        k = e->encodeNextTimeFrame();

    // Check stabilization, if stabilization is not guaranteed then unroll further if 'time' equals infinity.
    while ( e->findStabilizationCex(k) ) {
        printTime(stderr);
        fprintf(stderr, "The algorithm does not guarantee stabilization after %d time steps", k);

        if ( time < 0 ) k = e->encodeNextTimeFrame();
        else return 1;
    }

    printTime(stderr);
    fprintf(stderr, "The algorithm correctly stabilizes after %d time steps.", k);
    return 0;
}


// Find an algorithm
int find(int time, int overshoot) {
    if ( e->nextTimeFrameId() != 0 ) return 1; // Expects a "clean" Encoder

    fprintf(stderr, "Will search for a stabilizing algorithm with -time=");
    if ( time < 0 ) fprintf(stderr, "infinity"); else fprintf(stderr, "%d", time);
    fprintf(stderr, " and -overshoot=");
    if ( overshoot < 0 ) fprintf(stderr, "infinity"); else fprintf(stderr, "%d", overshoot);
    fprintf(stderr, ".\n");

    int k = 0;
    while( k < 1 )
        k = e->encodeNextTimeFrame();

    printTime(stderr);
    fprintf(stderr, "Enforcing base property");
    while ( e->findBasePropertyCex() )
        e->forbidLastCex();

    printTime(stderr);
    fprintf(stderr, "Searching for a self-stabilizing transition relation.");

    while( true ) {
        TTable* tt = e->findTransitionRelation();
        if ( !tt ) break;

        int  ilK = 0;
        bool cex = false;

        do {
            // Find minimal length illegal execution
            while ( !cex && ilK < k )
                cex = e->findLocalPropertyCex(++ilK, tt);

            if ( !cex ) {
                // If hitting Zero or One at timepoint k is unavoidable, we've found a stabilizing algorithm
                while ( !e->findStabilizationCex(k, tt) ) {
                    printResult(k, tt);
                    if ( time<0 || k<=time ) { e->deleteTable(tt); return RET_SAT; } // If stabilization happens 'fast enough' then we're done
                    overshoot = 0;
                    k--;
                }

                // If no loop was found, and we are not at the maximum unrolling depth, a new frame must be created
                if ( time<0 || overshoot<0 || k<overshoot+time ) {
                    k = e->encodeNextTimeFrame();
                    printTime(stderr);
                    fprintf(stderr, "Encoded timeframe k=%d.", k);
                }
                // Else, forbid the non stabilization counterexample
                else cex = true;
            }
        }
        while(!cex);

        // Forbid the last counterexample found. This invalidates the current transition relation
        e->forbidLastCex();
        e->deleteTable(tt);
    }

    printTime(stderr);
    fprintf(stderr, "A transition relation %sthat is self-stabilizing ", tableLoaded?"respecting the input file ":"");
    if ( time >= 0 && k >= time ) {
        fprintf(stderr, "in <=%d timesteps ", k);
    }
    fprintf(stderr, "does not exist.\n");

    return RET_UNSAT;
}


// Main function, reads parameters and starts the algorithm
int main(int argc, char** argv) {
    // Parse command line parameters
    parseOptions(argc, argv, true);

    signal(SIGINT, SIGINT_exit);
    signal(SIGXCPU,SIGINT_exit);

    if ( opt_faulty >= opt_nodes ) {
        fprintf(stderr, "ERROR: The number of faulty nodes must be smaller than the total number of nodes.");
        return RET_FAIL;
    }

    FILE *fp = (argc>1) ? fopen(argv[1], "rt"): NULL;
    if ( argc>1 && !fp ) {
        fprintf(stderr, "ERROR: Failed to open file '%s'.", argv[1]);
        return RET_FAIL;
    }

    if ( opt_verify && !fp)
        fprintf(stderr, "ERROR: Option '-verify' specified, but no input file given.");

    e = ((Encoder*) new PulseGenEncoder (opt_states, opt_nodes, opt_faulty, opt_cyclic));

    if ( fp && !enforceTransitionTable(fp) ) {
        fprintf(stderr, "ERROR: While parsing file '%s'.", argv[1]);
        return RET_FAIL;
    }

    int ret;
    if ( opt_verify )
        ret = verify((int) opt_time);
    else if ( opt_min )
        ret = find(0, -1);
    else
        ret = find((int) opt_time, (int) opt_overshoot);

    printStats(stderr);
    delete e;
    e = NULL;

    return ret;
}
