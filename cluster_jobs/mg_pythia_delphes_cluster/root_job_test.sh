#!/bin/bash

macro=GetTracks
max_ev=50000
root_file=/gpfs0/kats/users/wunch/cluster_out/dark_test.5000.0.5.0.50.150.500.root
dR=0.7
label=1
n_files=50

for ((j=0; j<n_files; j++))
do
root_file=/gpfs0/kats/users/wunch/cluster_out/dark$j.5000.0.5.0.50.150.500.root
qsub -q kats.q $HOME/dark_jets_repo/cluster_jobs/root_cluster/run_root.sh $macro "$root_file" $dR $label $max_ev "$root_file.$macro.$dR.txt"
done
