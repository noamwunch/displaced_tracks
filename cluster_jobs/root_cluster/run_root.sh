#!/bin/bash
export PATH="/gpfs0/kats/projects/clhep/install/bin:$PATH"
export PATH="/gpfs0/system/cmake-3.12.3-Linux-x86_64/bin/:$PATH"
export PATH="/gpfs0/kats/projects/root-build2/bin/:$PATH"
export PATH="/gpfs0/kats/projects/python-build/bin/:$PATH"
export PATH="/gpfs0/kats/projects/Python-3.4.5/:$PATH"
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

cd /gpfs0/kats/projects/Delphes-3.4.2
root -b << EOF
.x $HOME/dark_jets_repo/Root_macros/$1.C("$2", 0.7, $3, $4, "$5")
EOF

