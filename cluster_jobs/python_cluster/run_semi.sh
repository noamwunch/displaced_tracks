#!/bin/bash 

N=$1
epochs=$2
out_dir_specific=$3
ratio=$4

source $HOME/.bash_profile
conda activate dark_jets

python /gpfs0/kats/users/wunch/dark_jets_repo/Cluster_scripts/Semi-supervised.py "$out_dir_specific" $epochs $N $ratio 

conda deactivate
