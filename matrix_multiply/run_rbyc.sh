#!/bin/bash
gcc -g -o rbyc jl3752_mm_rbyc.c -O3
for((i=1024;i<=2048;i+=256));do
    ./rbyc $i
    echo $i
done
