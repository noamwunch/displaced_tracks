#!/bin/bash
N=150000
epochs=5
out_dir=Analysis/Semi-supervised/run2/

for ratio in 0.01 0.025 0.05 0.10 0.25 0.5
do
	qsub -q kats.q@sge1050 run_semi.sh $N $epochs "$out_dir$ratio/" $ratio
done

