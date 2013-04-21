Negative Results
================

This directory contains a proof that the following case is not
solvable by any algorithm:

  - s = 2 states
  - n = 4 nodes
  - f = 1 failures

There are two versions of the same proof, (a) and (b).

Proof (a) is longer, but it may be easier to follow, as it is
corresponds to the execution of a backtracking algorithm. Proof (b)
is much shorter.

Both of the proofs can be verified with a simple computer program;
see the directory `../code` for an example of a verification tool.


File Format
-----------

    S <size>
    R <list of state:node pairs>
    H <list of states>
    T <pattern>
    L <pattern>: <loop>
    L <pattern>: <loop>
    ...


Explanations and Examples
-------------------------

### S line

    S 243

The length of the proof: we consider 243 distinct cases ("L" rows).


### H and T lines

    H 0000 1000 0100 1100 0010 ... 1111
    T 1111 ?111 1?11 ???? 11?1 ... 0000

Trivial state transitions that we can fix without loss of generality.

For example, if we observe state 0010 (row H), we know that the new
state is 11?1 (row T).

That is, we know that nodes 0, 1, and 3 have to switch to state 1
(but any state transition is possible for node 2).

To see that this is necessary, consider the case that the algorithm
has stabilised, node 2 is faulty, and we are in configuration 00_0.
Now the observed state might be 0010, and we have to switch to
configuration 11_1.


### L lines

    L 1111 0111 1?11 0000 1101 ???0 ... 0000: _100 _010 _100
    ...

This is the main content of the file: a proof that no algorithm
exists. Refer to the "H" row for the column labels.

For each "L" row we give a *pattern* followed by a *loop*.

The *pattern* parts form a partition of the search space: each
algorithm that matches the pattern on the "T" row matches the pattern
on exactly one "L" row.

This can be verified efficiently e.g. as follows:

  1. Check that L patterns are subsets of the T pattern.
  2. Check that L patterns are mutually disjoint.
  3. Calculate how many algorithms match the T pattern.
  4. Calculate how many algorithms match the L patterns.
  5. Verify that the counts are equal.

The *loop* part is a concise proof of the following: all algorithms
that match the *pattern* are invalid.

More precisely, any such algorithm can be forced by the adversary to
enter the given loop. In the loop description, "_" denotes the faulty
node. In the above example, we have a loop of length 2:

  - starting from configuration _100
  - we can reach configuration _010
  - and then again configuration _100

Hence the algorithm fails to stabilise.

The loops can be verified efficiently: just make sure that each pair
of state transitions is correct. For example, to verify that
transition _100 -> _010 is feasible:

  - current configuration is _100
  - observed configuration for each node is 0100 or 1100
  - the relevant parts of the algorithm are:
  
        0100 -> 1?11
        1100 -> 0000
  
  - regardless of how we fix the transitions marked with "?":
  
      - we can force node 1 to observe 1100 and switch to state 0
      - we can force node 2 to observe 0100 and switch to state 1
      - we can force node 3 to observe 1100 and switch to state 0
  
  - in summary, from state _100 we can reach state _010.

Similarly, we can verify the part _010 -> _100.


### R lines

    R 0011:0 1100:0 1100:1 ...

The order in which we branched to generate this proof:

  - first choose the state transition for node 0 in observed state 0011
  - second choose the state transition for node 1 in observed state 1100
  - ...

This is not needed to verify the proof, it is given here for
reference.

The branching order was optimised by a local search heuristic with
random restarts.
