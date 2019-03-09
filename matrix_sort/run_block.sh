#!/bin/bash
rm sort_block
gcc -std=gnu99 -o sort_block -fopenmp jl3752_openmp_sort_block.c -O3
for((i=1;i<=20;i+=1));do
    ./sort_block 1000 100 4
done
