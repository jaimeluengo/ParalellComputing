#!/bin/bash
rm sort_block
gcc -std=gnu99 -o sort_block -fopenmp jl3752_hw2_openmp_sort_block.c -O3

for((i=1;i<=30;i+=1));do
    ./sort_block 1000 1000 8
done
echo wathsssapppp 
for((i=2;i<=10000;i*=2));do
    ./sort_block 256 $i 8
done
echo wathsssapppp 
for((i=2;i<=10000;i*=2));do
    ./sort_block $i 256 8
done
echo wathsssapppp 
for((i=2;i<=8000;i*=2));do
    ./sort_block $i $i 8
done

