#!/bin/bash
rm sort_colum
gcc -std=gnu99 -o sort_colum -fopenmp jl3752_openmp_sort_column.c -O3

for((i=1;i<=14;i+=1));do
    ./sort_colum 5000 5000 $i
done
echo wathsssapppp 
for((i=2;i<=1024;i*=2));do
    ./sort_colum 256 $i 8
done
echo wathsssapppp 
for((i=2;i<=1024;i*=2));do
    ./sort_colum $i $i 8
done
#for((i=1000;i<=50000;i+=1000));do
#    ./sort_colum $i $i 4
#done
