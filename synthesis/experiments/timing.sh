#!/bin/bash
TIME_FORMAT="realtime=%e usertime=%U rss=%M exitcode=%x" 
OUT_DIR=$1
RESULT_FILE="$OUT_DIR/results.txt"
shift 
mkdir -p $OUT_DIR
echo "$@" >> $RESULT_FILE
/usr/bin/time -f"$TIME_FORMAT" -o $RESULT_FILE -a "$@"
