import argparse
import os.path
import subprocess
import sys

INSTANCES_DIR = os.path.abspath("./instances/")
TIMING_PATH = os.path.abspath("./timing.sh")
SCRIPT_DIR = os.path.abspath("../direct/")
TOOLS_DIR = os.path.abspath("../tools/")

def do_timing(params, outdir, **sbparams):
    return subprocess.call([TIMING_PATH, outdir] + params, **sbparams)

def parse_arguments(args=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("-outdir")
    parser.add_argument("-solver")
    parser.add_argument("-seed")
    parser.add_argument("-nodes")
    parser.add_argument("-states")
    parser.add_argument("-faulty")
    parser.add_argument("-time", )
    parser.add_argument("-cyclic", action='store_true', default=False, required=False)
    parser.add_argument("-no-cyclic", action='store_true', default=True, required=False)
    return parser.parse_args(args)

def parse_gen_arguments(args=None):
    parser = argparse.ArgumentParser()
    parser.add_argument("-nodes", required=True)
    parser.add_argument("-states", required=True)
    parser.add_argument("-faulty", required=True)
    parser.add_argument("-time", required=True)
    parser.add_argument("-cyclic", action='store_true', default=False, required=False)
    parser.add_argument("-no-cyclic", action='store_true', default=True, required=False)
    return parser.parse_args(args)

def get_basename(args):
    base="{states}-{nodes}-{faulty}-{time}".format(**vars(args))
    if args.cyclic:
        base += "-c"
    return base

def run(encoding_str):
    # encoding str is an identifier for the encoding. e.g., 'simple'
    print " ".join(sys.argv)
    args = parse_arguments()
    base = get_basename(args)

    encdir = os.path.join(INSTANCES_DIR, encoding_str)
    cnffile = os.path.join(encdir, "cnf-"+get_basename(args))
    mapfile = os.path.join(encdir, "map-"+get_basename(args))

    solfile = os.path.join(args.outdir, "sol-"+get_basename(args))
    symsolfile = os.path.join(args.outdir, "ssol-"+get_basename(args))
    algfile = os.path.join(args.outdir, "alg-"+get_basename(args)+".txt")

    # Run solver
    solver = os.path.join(SCRIPT_DIR, "solve-%s" % args.solver)
    ret = do_timing([solver, str(args.seed).strip(), cnffile, solfile], args.outdir)

    # Map to algorithm if a solution was found
    if ret == 0:
	is_sat = subprocess.call(["grep", "-q", "^s SAT", solfile]) == 0
	is_unsat = subprocess.call(["grep", "-q", "^s UNSAT", solfile]) == 0
	if is_sat:
            print "Output SAT"
	    subprocess.call([os.path.join(TOOLS_DIR, "symcnf/result"), solfile, mapfile], stdout=open(symsolfile, 'w'))
            subprocess.call([os.path.join(SCRIPT_DIR, "print-alg")], stdin=open(symsolfile,'r'), stdout=open(algfile, 'w'))
        elif is_unsat:
            print "Output UNSAT"
        else:
            print "Output UNKNOWN"
