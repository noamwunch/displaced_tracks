#!/bin/bash
OutDir=/gpfs0/kats/users/wunch/cluster_out/
n_ev=10000
rinv=0.5
filename="$OutDir/dark_test2.$n_ev.$rinv.root"

qsub -q kats.q@sge1050 $HOME/dark_jets_repo/cluster_jobs/mg_pythia_delphes_cluster/main_to_root.sh $n_ev $filename 1 $rinv

