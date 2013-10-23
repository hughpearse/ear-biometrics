#!/bin/bash
#Segment leaf from uniform background color
for i in `ls -1 ./ear-dataset/`;
do
echo $i;
./process.out ./ear-dataset/$i >> ./10-metric-DB-opencv.arff
done

