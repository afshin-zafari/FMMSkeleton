#!/bin/bash

files=$(ls temp/temp/test*169*/1/rank*/stderr)
for f in $files
do
   if [ ! -e "${f}_dlb.txt" ] ; then 
     echo $f
     python extract.py $f >> extract.all
  
   fi
#    python extract.py $f >> extract.all
    echo -n "."
done
grep "full cycle" extract.all > extract_full_cycle.txt
