#!/bin/bash

n_files=10
n_ev=50000

for ((j=0; j<n_files; j++))
do	
	if [ ! -f "/gpfs0/kats/users/wunch/cluster_out/bb$j.root" ]; then
       		qsub -q kats.q@sge1050 $HOME/dark_jets_repo/cluster_jobs/mg_pythia_delphes_cluster/main_to_root.sh $n_ev "bb$j" 0
	fi

	if [ ! -f "/gpfs0/kats/users/wunch/cluster_out/dark$j.root" ]; then
		qsub -q kats.q@sge1050 $HOME/dark_jets_repo/cluster_jobs/mg_pythia_delphes_cluster/main_to_root.sh $n_ev "dark$j" 1
	fi      
done



