#!/bin/bash
n_files=10
n_ev=50000
for ((i=0; i<n_files; i++))
do	
        rm Settings.txt
        cp Settings_bb.txt Settings.txt
        cp ./Grid_bb.dat Grid.dat	 
	qsub -q kats.q $HOME/dark_jets_repo/cluster_jobs/mg_pythia_delphes_cluster/main_to_root.sh $n_ev "bb$i"

        rm Settings.txt
        cp Settings_dark.txt Settings.txt
        cp ./Grid_dark.dat Grid.dat
	qsub -q kats.q $HOME/dark_jets_repo/cluster_jobs/mg_pythia_delphes_cluster/main_to_root.sh $n_ev "dark$i"
done


