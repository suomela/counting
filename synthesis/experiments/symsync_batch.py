#! /usr/bin/env python
import os.path
import sys

PATH = os.path.expanduser("~/pulse-gen-experiments/symsync_wrap.py")
RANDOM_SEEDS = int(sys.argv[2])
SEED=int(sys.argv[3])
PREFIX_PATH = sys.argv[1] # os.path.expandvars("$WRKDIR/symsync-output/")
additional_args = sys.argv[4:]

from batch import *

def symsync_out(**params):
    prefix = "{path}/n{nodes}-s{states}-f{faulty}-t{time}-{cyclicity}-{id}-seed{seed}".format(path=PREFIX_PATH, **params)
    cmd = "{path} -outdir={prefix}/ -nodes={nodes} -states={states} -faulty={faulty} -time={time} -{cyclicity} -rnd-seed={seed} -rnd-freq={rnd_freq}".format(path=PATH, prefix=prefix, **params)
    if len(additional_args) > 0:
	cmd += " ".join(additional_args)
    return cmd

params_list = read_list(sys.stdin)
output_batch(params_list, symsync_out, sys.stdout, RANDOM_SEEDS, master_seed=SEED)
