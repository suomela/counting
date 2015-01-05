#! /usr/bin/env python
import os.path
import sys
from batch import *

encodingstr = sys.argv[1]
PATH = "./generate-%s" % (encodingstr,)

def gen_out(**params):
    cmd = "{path} -nodes={nodes} -faulty={faulty} -states={states} -time={time} -{cyclicity} ".format(path=PATH, **params)
    return cmd

import sys
params_list = read_list(sys.stdin)
output_batch(params_list, gen_out, sys.stdout)
