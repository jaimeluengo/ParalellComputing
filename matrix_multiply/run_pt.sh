#!/bin/bash
gcc -g -o pt jl3752_mm_pt.c -O3 -pthread
for((i=32;i<=6000;i*=2));do
    ./pt $i 4
done