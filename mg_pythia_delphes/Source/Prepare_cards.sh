#!/bin/bash

# Put the run card in the card folder
cd $MGPath/CaseIandII/Cards
cp $ScriptPath/Cards/run_card.dat run_card.dat

# Change the number of events to simulate
sed -i -e "s/ReplaceNEvents/$nEvents/g" run_card.dat

# Change the PDF
sed -i -e "s/ReplacePDF/$PDF/g" run_card.dat

# Change the beam energies
sed -i -e "s/ReplaceE1/$beamEnergy/g" run_card.dat
sed -i -e "s/ReplaceE2/$beamEnergy/g" run_card.dat
sed -i -e "s/ReplacepTMin/$pTMin/g" run_card.dat
sed -i -e "s/ReplacepTMax/$pTMax/g" run_card.dat

# Put the param_card in the card folder
if [ $model = Category_1_and_2_ns_UFO ]; then
cp $ScriptPath/Cards/param_card_ns.dat param_card.dat
else
cp $ScriptPath/Cards/param_card_nf.dat param_card.dat
fi

#Write mass information
sed -i -e "s/ReplacemQ/$mQ/g" param_card.dat
sed -i -e "s/ReplacemU/$mU/g" param_card.dat
sed -i -e "s/ReplacemD/$mD/g" param_card.dat
sed -i -e "s/ReplacemL/$mL/g" param_card.dat
sed -i -e "s/ReplacemE/$mE/g" param_card.dat
sed -i -e "s/ReplacemN/$mN/g" param_card.dat

cd $ScriptPath
