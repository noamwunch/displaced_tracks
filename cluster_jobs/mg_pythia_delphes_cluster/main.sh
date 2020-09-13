#!/bin/bash

#1) Read settings
. ./Source/Setting_reader.sh $2

echo "settings read"

#2) Preliminary steps
. ./Source/Preliminary_steps.sh

echo "perliminary steps"

#3) Start loop
for ((i=0; i < nPoints; i++))
do

#4) Mass reader
. ./Source/Mass_reader.sh

#5) Prepare cards
. ./Source/Prepare_cards.sh

#6) Run MadGraph
cd $MGPath/CaseIandII
./bin/madevent <<EOF
launch run_a$i
EOF
cd $ScriptPath

nameRun="run_a"

#7) Run Pythia
. ./Source/Pythia_runner.sh

echo "pythia done"

#8) Delphes
. ./Source/Delphes_applier.sh

#9) Root
. ./Source/Root_runner.sh

#10) End loop
. ./Source/End_loop.sh
done

#11) Concluding step
. ./Source/Final_step.sh $1
