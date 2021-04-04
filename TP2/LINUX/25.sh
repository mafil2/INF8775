#!/bin/bash

for n in {"5","10","15","20","25"}; do
    ./inst_gen.py -s $n -n 5 -x DP
done
