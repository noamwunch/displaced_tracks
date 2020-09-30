#!/bin/bash
source ~/.bash_profile

conda activate dark_jets
python /gpfs0/kats/users/wunch/dark_jets_repo/Cluster_scripts/Semi-supervised.py 1 2 3 4
conda deactivate
