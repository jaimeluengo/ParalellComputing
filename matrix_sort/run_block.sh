#!/bin/bash
rm sort_block
gcc -std=gnu99 -o sort_block -fopenmp jl3752_openmp_sort_block.c -O3
<<<<<<< HEAD

for((i=1;i<=30;i+=1));do
    ./sort_block 2000 2000 $i
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

=======
for((i=1;i<=20;i+=1));do
    ./sort_block 1000 100 4
done
>>>>>>> 298b843dd64cff77955a83ced07034ab9f1668ec
