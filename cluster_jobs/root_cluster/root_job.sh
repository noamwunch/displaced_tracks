#!/bin/bash

macro=GetTracks
max_ev=1000000

for root_file in /gpfs0/kats/users/wunch/cluster_out/bb*.root
do	
	qsub -q kats.q $HOME/dark_jets_repo/cluster_jobs/root_cluster/run_root.sh $macro "$root_file" 0 $max_ev "$root_file.$macro.txt"
done

for root_file in /gpfs0/kats/users/wunch/cluster_out/dark*.root
do	
	qsub -q kats.q $HOME/dark_jets_repo/cluster_jobs/root_cluster/run_root.sh $macro "$root_file" 1 $max_ev "$root_file.$macro.txt"
done

