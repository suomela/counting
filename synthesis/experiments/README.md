Experiments
===========

Setup
-----

The experiments have been run on a Linux computing cluster. To
re-create the batch files used for the experiments run:

    make setup
    make batches

Now the subdirectory `batches` should contain several batch files that
can be run. However, running all of them on a single CPU will take up
to several years of CPU time.

Please note that the utility script `timing.sh` uses the GNU `time`
executable.


Direct encoding experiments
---------------------------

First, it is necessary to create the CNF input files. This will take
some time:

    ./batches/batch-gen-TYPE.sh

where TYPE can be `realisable` or `unrealisable`.

To run all the experiments, simply execute all commands in the

    batch-SOLVER-TYPE.sh

files, where SOLVER is either `minisat` or `lingeling` and TYPE is as
given above. Note that this will take a *very* long time.


Symsync experiments
-------------------

To run the symsync experiments on realisable instances, run the
commands in the batch file

    batches/batch-symsync-overshoot-realisable.sh

The commands to run experiments on unrealisable instaces are given in

    batches/batch-symsync-unrealisable.sh
