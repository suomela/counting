#!/bin/sh

# Basic direct encoding

./generate-simple output 3 4 1 10 -- c
./solve output/cnf-3-4-1-10-c
../../code/verify-positive.py output/alg-3-4-1-10-c.txt

# The 'subtimes' encoder

./generate-subtimes output 3 5 2 10 -- c
./solve output/cnf-3-5-t-2-10-c 
mv output/alg-3-5-t-2-10-c.txt output/alg-3-5-1-10-c.txt
../../code/verify-positive.py output/alg-3-5-1-10-c.txt
