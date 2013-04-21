Synchronous Counting
====================

We have a complete communication network on _n_ nodes, each of which
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


Authors
-------

  - Danny Dolev, The Hebrew University of Jerusalem
  - Janne H. Korhonen, HIIT and University of Helsinki
  - Christoph Lenzen, MIT
  - Joel Rybicki, HIIT and University of Helsinki
  - Jukka Suomela, HIIT and University of Helsinki

This work was supported in part by the Israeli Centers of Research
Excellence (I-CORE) program, the Israeli Ministry of Science and
Technology, the Academy of Finland, the Research Funds of the
University of Helsinki, the Helsinki Doctoral Programme in Computer
Science – Advanced Computing and Intelligent Systems, the Swiss
Society of Friends of the Weizmann Institute of Science, and the
German Research Foundation (DFG).
