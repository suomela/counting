Positive Results
================

This directory contains the machine-generated algorithms.

All algorithms can be verified with a simple computer program;
see the directory `../code` for an example of a verification tool.


Naming Conventions
------------------

Cyclically symmetric algorithms:

    *-<s>-<n>-<f>-<t>-c.txt

Other algorithms:

    *-<s>-<n>-<f>-<t>.txt

Parameters:

  - s = number of states
  - n = number of nodes
  - f = number of failures
  - t = time to stabilise


File Format
-----------

The file format is very straightforward: we simply give the transition
table of the algorithm.

There are s^n lines of text, one line for each observed state.

Each line contains:

  - an observed state (n digits, range 0...s-1),
  - a space,
  - the new state for each node (n digits, range 0...s-1).

For example, the line

    0212 1000

indicates that if we observe the state vector 0212, then node 0
switches to state 1 and nodes 1..3 switch to state 0.
