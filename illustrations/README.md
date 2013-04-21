Illustrations
=============

`alg-3-4-1-7-c.pdf`: An illustration of the algorithm
`../positive/alg-3-4-1-7-c.txt`.

We show the *projection graph*, assuming that the first node is
faulty. The algorithm is cyclic, and hence all other projections are
isomorphic.

It can be verified that the only loop consists of the good
configurations *000 and *111. The configurations are clustered
according to the length of the longest directed path to a good
configuration. The longest path has length 7, and hence the algorithm
stabilises in time 7.
