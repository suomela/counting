Synchronous Counting
====================

This repository contains supporting material related to the following
paper:

  - Danny Dolev, Keijo Heljanko, Matti Järvisalo, Janne H. Korhonen,
    Christoph Lenzen, Joel Rybicki, Jukka Suomela, and Siert Wieringa:
    "Synchronous Counting and Computational Algorithm Design",
    Journal of Computer and System Sciences (to appear),
    http://arxiv.org/abs/1304.5719


Summary
-------

Consider a complete communication network on _n_ nodes, each of which
is a state machine with s states. In **synchronous 2-counting**, the
nodes receive a common clock pulse and they have to agree on which
pulses are "odd" and which are "even". We require that the solution is
*self-stabilising* (reaching the correct operation from any initial
state) and it tolerates f *Byzantine failures* (nodes that send
arbitrary misinformation).

In this repository we have computer-generated deterministic algorithms
that provide solutions to the following cases (see directory `positive`):

  - s = 3, n ≥ 4, f = 1
  - s = 2, n ≥ 6, f = 1

We also have a compact, computer-generated proof that the following
case is not solvable by any algorithm (see directory `negative`):

  - s = 2, n = 4, f = 1

We provide Python scripts that can be used to verify that the algorithms
and the lower-bound proofs are correct (see directory `code`).

For a quick start, try:

    cd code
    ./verify-all.sh

For some illustrations of the computer-generated algorithms, see the
directory `illustrations`.

Synthesis tools for finding computer-generated algorithms are given in
the `synthesis` directory.


Authors
-------

  - Danny Dolev, The Hebrew University of Jerusalem
  - Keijo Heljanko, HIIT and Aalto University
  - Matti Järvisalo HIIT and University of Helsinki
  - Janne H. Korhonen, HIIT and University of Helsinki
  - Christoph Lenzen, MPI Saarbrücken
  - Joel Rybicki, HIIT and Aalto University
  - Jukka Suomela, HIIT and Aalto University
  - Siert Wieringa, HIIT and Aalto University

