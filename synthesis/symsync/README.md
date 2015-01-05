Symsync
=======

Compiling
---------

The symsync tool depends on MiniSAT 2.2.0 which can be downloaded from
http://minisat.se/

Once you have the MiniSAT source code, set

    export MROOT=<minisat-dir>

and then run make in the symsync source code directory:

    make


Usage
-----
  
A summary of command line parameters available to symsync can be
printed using

    ./symsync --help-verb

There are two types of parameters. The parameters listed under
'CORE OPTIONS' are related to MiniSAT's search engine, whereas the
parameters under 'SymSync OPTIONS' are the input parameters for
symsync.


Example
-------

The following command starts a search for a 3-state cyclic algorithm
for 4 nodes and 1 faulty node with stabilisation time 15:

    ./symsync  -states=3 -nodes=4 -faulty=1 -time=15 -cyclic 

The resulting algorithm is written into `alg-3-4-1-10-c.txt`.
