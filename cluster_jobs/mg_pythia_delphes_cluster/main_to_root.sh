#!/bin/bash
export PATH="/gpfs0/kats/projects/clhep/install/bin:$PATH"
export PATH="/gpfs0/system/cmake-3.12.3-Linux-x86_64/bin/:$PATH"
export PATH="/gpfs0/kats/projects/root-build2/bin/:$PATH"
export PATH="/gpfs0/kats/projects/python-build/bin/:$PATH"
export PATH="/gpfs0/kats/projects/Python-3.4.5/:$PATH"
export PATH="/usr/bin/:$PATH"
export PKG_CONFIG_PATH="/gpfs0/kats/projects/rave/lib/pkgconfig"
export LIBRARY_PATH="/gpfs0/kats/projects/clhep/install/lib:$LIBRARY_PATH"
export LIBRARY_PATH="/gpfs0/kats/projects/rave/lib:$LIBRARY_PATH"
export CPATH="/gpfs0/kats/projects/clhep/install/include:$CPATH"
export CPATH="/gpfs0/kats/projects/rave/include/rave:$CPATH" 
export LD_LIBRARY_PATH="/usr/lib64:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="/gpfs0/kats/projects/clhep/install/lib:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="/gpfs0/kats/projects/root-build2/lib:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="/gpfs0/kats/projects/rave/lib:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="/gpfs0/kats/projects/Delphes-3.4.2:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="/gpfs0/kats/projects/python-build/lib:$LD_LIBRARY_PATH"
export ROOT_DIR="/gpfs0/kats/projects/root-build2:$LD_LIBRARY_PATH"
export CMAKE_PREFIX_PATH="/gpfs0/kats/projects/root-build2:$LD_LIBRARY_PATH"
export ROOT_INCLUDE_PATH="/gpfs0/kats/projects/rave/include:$ROOT_INCLUDE_PATH"
#stty erase '^?'
#stty kill '^U'

#1) Read settings
. ./Source/Setting_reader.sh $1

echo "settings read"

#2) Preliminary steps
. ./Source/Preliminary_steps.sh
echo "perliminary steps"

#3) Start loop
for ((i=0; i < nPoints; i++))
do

#4) Mass reader
. ./Source/Mass_reader.sh
echo "mass reader"

#5) Prepare cards
. ./Source/Prepare_cards.sh
echo "prepare cards"

#6) Run MadGraph
cd $ScriptPath/CaseIandII
/usr/bin/python2.7 $ScriptPath/CaseIandII/bin/madevent <<EOF
set fortran_compiler /usr/bin/gfortran
launch run_a$i
EOF
cd $ScriptPath

nameRun="run_a"

#7) Run Pythia
. ./Source/Pythia_runner.sh

echo "pythia done"

#8) Delphes
. ./Source/Delphes_applier.sh "$2"

#10) End loop
. ./Source/End_loop.sh
done

#11) Concluding step
. ./Source/Final_step.sh
