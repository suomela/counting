#! /usr/bin/env python
import subprocess as sp
import os.path 
import sys
import utils
# Call symsync and store results..
# Create output directories and call symsync

PARAMETERS = [x.replace("-outdir", "-algdir") for x in sys.argv[1:]]
SYMSYNC_PATH = "../symsync/symsync"
OUT_DIR = [x.split("=")[1] for x in PARAMETERS if x.find("-algdir") >= 0][0]
RESULT_FILE = os.path.join(OUT_DIR, "results.txt")
STDERR_FILE = os.path.join(OUT_DIR, "stderr.txt")

# Create output dir
retcode = sp.call(["mkdir", "-p", OUT_DIR])

# Call symsync via timing script
errf = open(STDERR_FILE, 'w')
utils.do_timing([SYMSYNC_PATH] + PARAMETERS, OUT_DIR, stderr=errf)
errf.close()
