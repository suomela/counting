#!/usr/bin/env python

import itertools
import os.path
import re
import sys
import Counting

# Naming convention: alg-S-N-F-T.txt or alg-S-N-F-T-c.txt
RE_FILENAME = re.compile(r"[a-z]+-(\d+)-(\d+)-(\d+)-(\d+)(-c)?\.txt")

class Projection:
    def __init__(self, alg, f):
        self.alg = alg
        self.f = f
        self.util = alg.util

        # All possible actual states
        self.as_list = []
        seen = set()
        for os1 in self.util.os_list:
            as1 = self.util.projection(os1, self.f)
            ast = tuple(as1)
            if ast not in seen:
                self.as_list.append(as1)
                seen.add(ast)

        if f is None:
            correct = self.alg.N
        else:
            correct = self.alg.N - 1
        assert len(self.as_list) == self.alg.S ** correct

        self.as_map = dict()
        for i, as1 in enumerate(self.as_list):
            self.as_map[tuple(as1)] = i

        # Reachability
        self.successors = []
        for as1 in self.as_list:
            # byz = all possible observed states for actual state "as1"
            byz = self.util.fix_all(as1, f)
            # j in new[i]  <=>
            # adversary can force node i to switch to state j
            new = []
            for n in range(self.alg.N):
                if n == f:
                    new.append([None])
                else:
                    l = [ self.alg.get_transition(b)[n] for b in byz ]
                    new.append(sorted(set(l)))
            # as2 in as2set  <=>
            # adversary can force state transition as1 -> as2
            as2set = set()
            for as2 in itertools.product(*new):
                i = self.as_map[tuple(as2)]
                as2set.add(i)
            self.successors.append(as2set)

    def verify(self):
        # good and bad states
        os_good = [ self.util.all_same(s) for s in (0, 1) ]
        as_good = [ self.util.projection(os1, self.f) for os1 in os_good ]
        i_good = [ self.as_map[tuple(as1)] for as1 in as_good ]
        i_all = range(len(self.as_list))
        i_bad = [ i for i in i_all if i not in i_good ]
        set_good = set(i_good)

        # counting after stabilisation
        assert self.successors[i_good[0]] == set([i_good[1]]), "count 0 -> 1"
        assert self.successors[i_good[1]] == set([i_good[0]]), "count 1 -> 0"

        # reachable[i] = what is reachable in time t from state i
        reachable = [ set([i]) for i in i_all ]
        t = 0
        while True:
            # already stabilised?
            stable = True
            for i in i_bad:
                if not reachable[i].issubset(set_good):
                    stable = False
            if stable:
                return t
            # not yet, one more step
            rnext = [ set() for i in i_all ]
            for i in i_all:
                for j in reachable[i]:
                    rnext[i].update(self.successors[j])
            reachable, t = rnext, t+1
            # verify that we did not create a loop
            for i in i_bad:
                assert i not in reachable[i], "no loop"

class Alg:
    def __init__(self, S, N, F, T, C):
        # To keep this script easy to follow,
        # we only support the case of one faulty node
        assert F == 1
        self.S = S
        self.N = N
        self.F = F
        self.T = T
        self.C = C
        self.util = Counting.Util(S=S, N=N)
        self.transitions = [ None for i in range(S ** N) ]

    def read_input(self, filename):
        with open(filename) as f:
            for l in f:
                a, b = self.util.parse_state_list(l.rstrip())
                i = self.util.os_map[tuple(a)]
                assert self.transitions[i] is None, "no duplicates"
                self.transitions[i] = b
        for b in self.transitions:
            assert b is not None, "all transitions defined"

    def verify(self):
        if self.C:
            self.verify_cyclic()
        self.verify_projection(None)
        for f in range(self.N):
            self.verify_projection(f)

    def verify_projection(self, f):
        if f is None:
            print "- no node is faulty:",
        else:
            print "- node %u is faulty:" % f,
        proj = Projection(self, f)
        t = proj.verify()
        assert t <= self.T, "stabilisation time"
        print "OK (t = %u)." % t

    def verify_cyclic(self):
        print "- cyclically symmetric:",
        for a in self.util.os_list:
            b = self.get_transition(a)
            for n in range(1, self.N):
                a2 = self.util.rot(a, n)
                b2 = self.util.rot(b, n)
                assert self.get_transition(a2) == b2, "symmetric"

        print "OK."

    def get_transition(self, v):
        i = self.util.os_map[tuple(v)]
        return self.transitions[i]

def verify(filename):
    basename = os.path.basename(filename)
    print "verifying: %s" % basename
    m = RE_FILENAME.match(basename)
    assert m is not None, "correct file name"
    S, N, F, T = [ int(m.group(i)) for i in (1,2,3,4) ]
    C = m.group(5) is not None
    alg = Alg(S=S, N=N, F=F, T=T, C=C)
    alg.read_input(filename)
    alg.verify()
    print

for filename in sys.argv[1:]:
    verify(filename)
