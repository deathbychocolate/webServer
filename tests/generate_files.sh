#!/bin/bash

for (( n = 1; $n <= 128; n = $n + 1 ))
do
    fileName="$n"
    fileName+="kb.txt"
    dd if=/dev/zero of=$fileName bs=1k count=$n
done