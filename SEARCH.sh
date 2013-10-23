#!/bin/bash
#Segment leaf from uniform background color

metrics=`./process.out $1 | cut -d',' -f -10`;
file=`java -jar search.jar $metrics /home/hughpear/Desktop/biometrics/10-metric-DB-opencv.arff`;
echo "$file";
eog $file;

