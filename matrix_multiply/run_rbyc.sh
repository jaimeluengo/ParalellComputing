#!/bin/bash
for((i=16;i<=6000;i*=2));do
    ./rbyc $i
    echo $i
done