#!/bin/bash
gcc -g -o ./tile jl3752_mm_tile.c -O3
for((i=32;i<=6000;i*=2));do
    ./tile $i 32
done