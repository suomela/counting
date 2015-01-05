#! /usr/bin/env python
import os.path
import sys

PATH = sys.argv[1] 
SOLVER = sys.argv[5]
SEED = int(sys.argv[4])
RANDOM_SEEDS = int(sys.argv[3])
PREFIX_PATH = sys.argv[2]

from batch import *

def cmd_out(**params):
    prefix = "{path}/n{nodes}-s{states}-f{faulty}-t{time}-{cyclicity}-{id}-seed{seed}".format(path=PREFIX_PATH, **params)
    cmd = "{path} -solver={solver} -outdir={prefix} -nodes={nodes} -states={states} -time={time} -faulty={faulty} -{cyclicity} -seed={seed}".format(path=PATH, solver=SOLVER, prefix=prefix, **params)
    return cmd

import sys
params_list = read_list(sys.stdin)
output_batch(params_list, cmd_out, sys.stdout, RANDOM_SEEDS, master_seed=SEED)
