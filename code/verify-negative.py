#!/usr/bin/env python3

import os.path
import sys
import Counting

S = 2
N = 4
F = 1


### Proof verification

class Proof:
    def __init__(self):
        self.util = Counting.Util(S=S, N=N)
        self.header = None
        self.trivial = None
        self.loops = []

    def read_input(self, filename):
        with open(filename) as f:
            for l in f:
                l = l.rstrip()
                assert l[0] in "HTLSR", "row label"
                assert l[1] == " ", "label separator"
                tail = l[2:]
                if l[0] == "H":
                    assert self.header is None, "H line given only once"
                    self.header = self.util.parse_state_list(tail)
                    assert self.header == self.util.os_list, "H line correct"
                elif l[0] == "T":
                    assert self.trivial is None, "T line given only once"
                    self.trivial = self.util.parse_state_list(tail, missing="?")
                    assert len(self.trivial) == S**N, "T line length correct"
                elif l[0] == "L":
                    salg, sloop = tail.split(": ")
                    alg = self.util.parse_state_list(salg, missing="?")
                    loop = self.util.parse_state_list(sloop, missing="_")
                    assert len(alg) == S**N, "L line length correct"
                    self.loops.append((alg, loop))
                else:
                    pass
        assert self.header is not None, "H line exists"
        assert self.trivial is not None, "T line exists"
        assert len(self.loops) > 0, "at least one L line given"

    ### Verification

    def verify(self):
        self.verify_trivial()
        self.verify_exhaustive()
        self.verify_loops()

    #--- Trivial state transitions are correct

    def verify_trivial(self):
        print("- trivial transitions:", end=' ')
        for i, os in enumerate(self.util.os_list):
            for n in range(N):
                if self.trivial[i][n] is not None:
                    assert self.trivial[i][n] == 1 - os[n], "correct new state"
                    same = 0
                    for x in os:
                        if x == os[n]:
                            same += 1
                    assert same >= N - 1, "enough similar old states"
        print("OK.")

    #--- All cases are covered

    def verify_exhaustive(self):
        target = self.util.alg_string(self.trivial)
        all_algs = [ self.util.alg_string(x[0]) for x in self.loops ]
        print("- candidates are disjoint:", end=' ')
        for i in range(len(all_algs)):
            for j in range(i):
                self.verify_disjoint(all_algs[i], all_algs[j])
        print("OK.")
        print("- list is exhaustive:", end=' ')
        count = 0
        for alg in all_algs:
            self.verify_subset(alg, target)
            count += 2**self.count_alg(alg)
        expected = self.count_alg(target)
        assert count == 2**expected, "coverage"
        print("OK.")

    # Algorithm patterns alg1 and alg2 are disjoint
    # (no algorithm is covered by both alg1 and alg2)
    def verify_disjoint(self, alg1, alg2):
        assert len(alg1) == len(alg2)
        for i in range(len(alg1)):
            if alg1[i] != alg2[i] and alg1[i] in "01" and alg2[i] in "01":
                return
        assert False, "disjoint"

    # Algorithm pattern alg1 is a subset of alg2
    # (any algorithm covered by alg1 is also covered by alg2)
    def verify_subset(self, alg1, alg2):
        assert len(alg1) == len(alg2)
        for i in range(len(alg1)):
            if alg1[i] != alg2[i]:
                assert alg1[i] in "01", "subset"
                assert alg2[i] == "?", "subset"

    # Count how many algorithms are covered by pattern alg
    # (returns logarithm)
    def count_alg(self, alg):
        q = 0
        for x in alg:
            if x == "?":
                q += 1
        return q


    #--- Each loop is correct

    def verify_loops(self):
        print("- loops are correct:", end=' ')
        for alg, loop in self.loops:
            self.verify_one_loop(alg, loop)
        print("OK.")

    def verify_one_loop(self, alg, loop):
        assert len(loop) >= 2, "loop length at least 1"
        assert loop[0] == loop[-1], "loop is closed"
        for i in range(len(loop) - 1):
            self.verify_step(alg, loop[i], loop[i+1])

    def verify_step(self, alg, as1, as2):
        # Extract the identity of the Byzantine node,
        # make sure it is well-defiend
        n1 = self.util.nones(as1)
        n2 = self.util.nones(as2)
        assert n1 == n2, "same faulty node"
        assert len(n1) == 1, "one faulty node"
        f = n1[0]
        # What are possible observed states in actual state "as1"
        byz = []
        for s in range(S):
            os = tuple(self.util.fix(as1, f, s))
            byz.append(self.util.os_map[os])
        # Make sure we can make a move to "as2"
        for n in range(N):
            if n != f:
                # possible_moves may contain Nones
                possible_moves = [ alg[i][n] for i in byz ] 
                claimed_move = as2[n]
                assert claimed_move in possible_moves, \
                    "loop transition is possible"


def verify(filename):
    print("verifying: %s" % os.path.basename(filename))
    proof = Proof()
    proof.read_input(filename)
    proof.verify()
    print()

for filename in sys.argv[1:]:
    verify(filename)
