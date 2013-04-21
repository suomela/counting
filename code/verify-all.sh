#!/bin/sh

./verify-positive.py ../positive/alg-*.txt || exit 1
./verify-negative.py ../negative/proof-*.txt || exit 1

echo "All proofs OK."
