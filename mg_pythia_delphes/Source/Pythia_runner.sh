#!/bin/bash

# Transfer events to Pythia
cd $MGPath/CaseIandII/Events/$nameRun$i
gzip -d unweighted_events.lhe.gz
mv unweighted_events.lhe Hidden_valley_6.lhe
cp Hidden_valley_6.lhe $PythiaPath/examples

# Implement rinv
rvis=$(echo "x=1-$rinv; print 0; x" | bc)
cd $ScriptPath/Cards
cp main_hidden_valley_6.cmd $PythiaPath/examples
cd $PythiaPath/examples
sed -i -e "s/rinv/$rinv/g" main_hidden_valley_6.cmd
sed -i -e "s/rvis/$rvis/g" main_hidden_valley_6.cmd

# Introduce new parameters
sed -i -e "s/Removenfl/$nf025ns/g" main_hidden_valley_6.cmd
sed -i -e "s/RemovenGauge /$nGauge/g" main_hidden_valley_6.cmd
sed -i -e "s/RemovelambdaD/$lambdaD/g" main_hidden_valley_6.cmd
sed -i -e "s/RemoveDquarkMass/$DquarkMass/g" main_hidden_valley_6.cmd
sed -i -e "s/RemoveDpionMass/$DpionMass/g" main_hidden_valley_6.cmd
sed -i -e "s/RemoveDRhoMass/$DrhoMass/g" main_hidden_valley_6.cmd
sed -i -e "s/RemoveProbV/$ProbV/g" main_hidden_valley_6.cmd
sed -i -e "s/RemovedecayLength/$decayLength/g" main_hidden_valley_6.cmd

# Calculate pTmin for showering and replace it in code
pTminS=$(echo "x=1.1*$lambdaD; x" | bc)
sed -i -e "s/RemovepTmin/$pTminS/g" main_hidden_valley_6.cmd

# Write spin correctly
if [ $model = Category_1_and_2_ns_UFO ]; then
sed -i -e "s/RemovespinFV/1/g" main_hidden_valley_6.cmd
sed -i -e "s/Removespinqv/0/g" main_hidden_valley_6.cmd
else
sed -i -e "s/RemovespinFV/0/g" main_hidden_valley_6.cmd
sed -i -e "s/Removespinqv/1/g" main_hidden_valley_6.cmd
fi

# Run Pythia
./main_Hidden_valley_6

cd $ScriptPath
