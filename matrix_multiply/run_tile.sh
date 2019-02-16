#!/bin/bash
gcc -g -o ./tile jl3752_mm_tile.c -O3
for((i=1;i<=2048;i*=2));do
    ./tile 2048 $i
done
