#!/bin/bash

# Transfer card to Delphes and run Delphes
cd $DelphesPath
rm results.root
./DelphesHepMC cards/Delphes_resolution.tcl results.root $PythiaPath/examples/main_Hidden_valley_6.hepmc

cd $ScriptPath
