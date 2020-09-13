#!/bin/bash

cd $ScriptPath

mQ=$(awk 'FNR==1 {print $1}' Grid_temp.dat)
mU=$(awk 'FNR==1 {print $2}' Grid_temp.dat)
mD=$(awk 'FNR==1 {print $3}' Grid_temp.dat)
mL=$(awk 'FNR==1 {print $4}' Grid_temp.dat)
mE=$(awk 'FNR==1 {print $5}' Grid_temp.dat)
mN=$(awk 'FNR==1 {print $6}' Grid_temp.dat)
rinv=$(awk 'FNR==1 {print $7}' Grid_temp.dat)

cd $ScriptPath
