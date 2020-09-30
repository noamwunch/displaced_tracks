#!/bin/bash
OutDir=/gpfs0/kats/users/wunch/cluster_out/
n_files=10
n_ev=50000
rinv=0.5

for ((j=0; j<n_files; j++))
do	
	filename="$OutDir/bb$j.$n_ev.$rinv.root"
	if [ ! -f $filename ]; then
       		qsub -q kats.q@sge1050 $HOME/dark_jets_repo/cluster_jobs/mg_pythia_delphes_cluster/main_to_root.sh $n_ev $filename 0 $rinv
	fi
	filename="$OutDir/dark$j.$n_ev.$rinv.root"
	if [ ! -f $filename ]; then
		qsub -q kats.q@sge1050 $HOME/dark_jets_repo/cluster_jobs/mg_pythia_delphes_cluster/main_to_root.sh $n_ev $filename 1 $rinv
	fi      
done



