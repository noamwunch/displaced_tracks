#!/bin/bash

macro=RaveTest_cluster
max_ev=10
root_file=/gpfs0/kats/users/wunch/cluster_out/bb1.root
dR=0.7
label=0

qsub -q kats.q $HOME/dark_jets_repo/cluster_jobs/root_cluster/run_root.sh $macro "$root_file" $dR $label $max_ev "$root_file.$macro.txt"

