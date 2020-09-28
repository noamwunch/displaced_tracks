#!/bin/bash

macro=RaveTest_cluster
max_ev=1000000

root_file=/gpfs0/kats/users/wunch/cluster_out/bb1.root
label=0
for dR in 0.5 0.7 1.0 1.5
do
	qsub -q kats.q $HOME/dark_jets_repo/cluster_jobs/root_cluster/run_root.sh $macro "$root_file" $dR $label $max_ev "$root_file.$macro.$dR.txt"
done

root_file=/gpfs0/kats/users/wunch/cluster_out/dark1.root
label=1
for dR in 0.5 0.7 1.0 1.5
do
	qsub -q kats.q $HOME/dark_jets_repo/cluster_jobs/root_cluster/run_root.sh $macro "$root_file" $dR $label $max_ev "$root_file.$macro.$dR.txt"
done



