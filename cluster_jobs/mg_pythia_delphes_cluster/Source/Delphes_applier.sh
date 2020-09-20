#!/bin/bash

# Transfer card to Delphes and run Delphes
cd $DelphesPath

./DelphesHepMC $ScriptPath/delphes_cards/Delphes_resolution.tcl $OutPath/$1.root $ScriptPath/examples/main_Hidden_valley_6.hepmc

cd $ScriptPath
