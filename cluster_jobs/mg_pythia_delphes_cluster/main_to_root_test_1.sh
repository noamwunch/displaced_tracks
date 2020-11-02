#!/bin/bash
OutDir=/gpfs0/kats/users/wunch/cluster_out/bb

n_ev=5000
rinv=0.5
ProbV=0.50
pTMin=150
pTMax=200

macro=GetTracks
max_ev=50000
dR=0.7
label=0

first_file=0
n_files=200

mkdir -p $OutDir

for ((j=first_file; j<(first_file+n_files); j++))
do
job_name="label${label}number${j}"	
filename="$OutDir/$label.$j.$n_ev.$rinv.$ProbV.$pTMin.$pTMax.root"

root_file=$OutDir/$label.$j.$n_ev.$rinv.$ProbV.$pTMin.$pTMax.root
qsub -q kats.q $HOME/dark_jets_repo/cluster_jobs/root_cluster/run_root.sh $macro "$root_file" $dR $label $max_ev "$root_file.$macro.$dR.txt"
done

