#!/bin/bash
source ~/.bash_profile

conda activate dark_jets
python py_test.py
conda deactivate
