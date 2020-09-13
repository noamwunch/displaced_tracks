#!/bin/bash

# Remove temporary grid
rm Grid_temp.dat

# Write completion time
cd $ResultPath

cp /home/noamwunch/Jet_tagging/Delphes-3.4.2/results.root /mnt/c/Users/noamw/Desktop/jet_tagging/Data/Results_root/$1

echo "" >> Summary.txt
echo "Completion time" >> Summary.txt
echo $(date) >> Summary.txt
