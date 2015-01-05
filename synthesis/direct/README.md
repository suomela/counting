Direct SAT encoding
===================

This directory contains scripts for generating synthesis instances
for synchronous 2-counting using the direct SAT encoding approach.

See `examples.sh` for examples on how to use the scripts.


Encoder generate-simple
-----------------------

Generate a basic direct CNF encoding using projection graphs.

Usage: 

    ./generate-simple outdir s n f t -- flags

Flags can either be empty or `c`. In the latter case, the synthesis
instance is restricted to the class of cyclic algorithms.

For example, the command

    ./generate-simple output 3 4 1 10 -- c

generates a CNF file into directory `output` that is satisfiable
iff s=3 n=4 f=1 t=10 for cyclic algorithms is realisable.


Encoder generate-subtimes
-------------------------

Generate a CNF encoding with specific stabilisation times for
different numbers of *actual* Byzantine nodes. That is, given a list
of stabilisation times t_0, ..., t_f, the resulting CNF encoding
enforces for each i = 0, ..., f that the algorithm stabilises in t_i
steps if there are i Byzantine nodes.

Usage:

    ./generate-sat outdir s n t_0 t_1 ... t_f -- flags

For example, the command

    ./generate-subtimes output 3 4 3 10

will create a CNF instance for s=3 states and n=4 nodes that
stabilises in 3 rounds if there are no faults and 10 rounds if there
is one faulty node.
