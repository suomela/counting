import numpy.random as rng
import itertools as it

import ast

# ---- Read a dict input ----

def read_list(f):
    ls = []
    for line in f:
        ls.append( ast.literal_eval(line) )
    return ls

# ---- Random seeds

def random_seeds(n):
    MAX_INT = 2**32 - 1 # FIXME 
    return rng.randint(1, MAX_INT, n)

# ----- Generate batch --------

def dict_product(d):
    """ Generate a Cartesian product from a dict of lists """
    pairs = it.product(*d.itervalues())
    return (dict(it.izip(d, p)) for p in pairs)

def output_batch(combinations, output_func, output_stream, n_seeds=1, master_seed=None):
    if master_seed is not None:
        rng.seed(master_seed)

    rng.shuffle(combinations)

    task_id = 0
    for p in combinations:
        for i in xrange(n_seeds):
            task_id += 1
            seed = random_seeds(1)[0]
            p['seed'] = seed
            p['id'] = task_id
            output_stream.write( output_func(**p) )
            output_stream.write('\n')


