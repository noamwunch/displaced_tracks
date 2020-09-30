#!/bin/bash

macro=GetTracks
max_ev=50000
root_file=/gpfs0/kats/users/wunch/cluster_out/dark_test2.10000.0.5.root
dR=0.7
label=1

qsub -q kats.q $HOME/dark_jets_repo/cluster_jobs/root_cluster/run_root.sh $macro "$root_file" $dR $label $max_ev "$root_file.$macro.txt"

