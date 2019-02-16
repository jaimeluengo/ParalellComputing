#!/bin/bash
gcc -g -o pt jl3752_mm_pt.c -O3 -pthread
for((i=1;i<=24;i++));do
    ./pt 4096 $i
done
