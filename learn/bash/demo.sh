#!/bin/bash 
x=1
while [ $x -lt 5 ]
do
let "x = $x + 1"
ps -ef | grep main 
done
