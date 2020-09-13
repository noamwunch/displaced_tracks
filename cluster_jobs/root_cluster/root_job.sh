#!/bin/bash

for root_file in /gpfs0/kats/users/wunch/dark_jets/results_root/bb*
do	
	qsub -q kats.q $HOME/dark_jets/run_root.sh "$root_file" 0 100000 "$root_file.txt"
done

for root_file in /gpfs0/kats/users/wunch/dark_jets/results_root/dark*
do	
	qsub -q kats.q $HOME/dark_jets/run_root.sh "$root_file" 1 100000 "$root_file.txt"
done

