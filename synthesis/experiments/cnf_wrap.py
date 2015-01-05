#! /usr/bin/env python
import subprocess as sp
import os.path 
import sys
import wrap_parser

def params_to_fname(encoding_str, params):
    return "cnf-{}-{nodes}-{states}-{time}-{cyclic}".format(encoding_str, **params)

args = wrap_parser.parse_arguments()

OUT_DIR = args.outputdir
RESULT_FILE = os.path.join(OUT_DIR, "result.txt")
STDOUT_FILE = os.path.join(OUT_DIR, "stdout.txt")
TIME_FORMAT = "realtime=%e usertime=%U rss=%M exitcode=%x" 

# Create output dir
retcode = sp.call(["mkdir", "-p", OUT_DIR])

parameters_str = " ".join(sys.argv)
print parameters_str
# Store input parameters
f = open(RESULT_FILE, 'w')
f.write(parameters_str + "\n")
f.close()

source = # FIXME
dest = source+"-solution"
seed = 
CMD = ["./run-lingeling.sh", source, dest, seed]

# Call symsync
errf = open(STDERR_FILE, 'w')
sp.call(["/usr/bin/time", "-p", "-o", RESULT_FILE, "-a", CMD, stderr = errf)
errf.close()
