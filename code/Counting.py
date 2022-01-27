### States as strings / lists

import itertools

class Util:
    def __init__(self, S, N):
        assert S >= 2
        assert N >= 4
        self.S = S
        self.N = N
        self.os_list = self.all_obs_states()
        self.os_map = dict()
        for i, os in enumerate(self.os_list):
            self.os_map[tuple(os)] = i

    # Individual states

    def state_string1(self, s, missing=None):
        if s is None:
            assert missing is not None
            return missing
        else:
            assert 0 <= s < self.S
            return str(s)

    def parse_state1(self, s, missing=None):
        if missing is not None and s == missing:
            return None
        else:
            v = int(s)
            assert 0 <= v < self.S
            return v

    # State vectors

    def state_string(self, v, missing=None):
        assert len(v) == self.N
        return "".join([self.state_string1(x, missing) for x in v])

    def parse_state(self, s, missing=None):
        assert len(s) == self.N
        return [ self.parse_state1(x, missing) for x in s ]

    def nones(self, v):
        assert len(v) == self.N
        return [ i for i in range(len(v)) if v[i] is None ]

    def rot(self, v, n):
        assert len(v) == self.N
        assert 0 <= n < self.N
        return v[n:] + v[:n]

    def projection(self, v, f):
        assert len(v) == self.N
        return [ None if i == f else v[i] for i in range(self.N) ]

    def fix(self, v, f, s):
        assert len(v) == self.N
        if f is not None:
            assert v[f] is None
        return [ s if i == f else v[i] for i in range(self.N) ]

    def fix_all(self, v, f):
        assert len(v) == self.N
        if f is None:
            return [ v ]
        else:
            return [ self.fix(v, f, s) for s in range(self.S) ]

    def all_same(self, s):
        return [ s for i in range(self.N) ]

    def all_obs_states(self):
        p = itertools.product(list(range(self.S)), repeat=self.N)
        return [ list(reversed(x)) for x in p ]

    def parse_state_list(self, s, missing=None):
        l = [ self.parse_state(x, missing) for x in s.split(" ") ]
        for v in l:
            assert len(v) == self.N
        return l

    # Algorithms as long strings

    def alg_string(self, v):
        assert len(v) == self.S ** self.N
        return "".join([self.state_string(x, missing="?") for x in v])

# Sanity checking

def check():
    u = Util(S=2, N=4)

    def verify(v, s, missing):
        assert u.state_string(v, missing) == s
        assert u.parse_state(s, missing) == v

    verify([0,1,0,0], "0100", None)
    verify([0,1,None,0], "01_0", "_")
    verify([0,1,None,0], "01?0", "?")

    assert u.projection([0,1,1,1], None) == [0,1,1,1]
    assert u.projection([0,1,1,1], 0) == [None,1,1,1]
    assert u.projection([0,1,1,1], 1) == [0,None,1,1]
    assert u.fix([0,1,1,1], None, 1) == [0,1,1,1]
    assert u.fix([0,1,None,1], 2, 1) == [0,1,1,1]
    assert u.fix_all([0,1,1,1], None) == [ [0,1,1,1] ]
    assert u.fix_all([0,1,None,1], 2) == [ [0,1,0,1],  [0,1,1,1] ]

    u = Util(S=5, N=5)
    assert u.rot([0,1,2,3,4], 0) == [0,1,2,3,4]
    assert u.rot([0,1,2,3,4], 1) == [1,2,3,4,0]
    assert u.rot([0,1,2,3,4], 2) == [2,3,4,0,1]

    u = Util(S=2, N=5)
    assert u.nones([0,0,None,1,None]) == [2,4]

check()
