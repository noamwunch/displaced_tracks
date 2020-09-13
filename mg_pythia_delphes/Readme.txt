####################
#   Explanations   #
####################

This folder contains a program to generate events.

The main script is main.sh. Simply execute it to run the program.

Settings are read from Setting.txt. It is possible to store events or not. Paths must be included in this file. The number of events to be generated for each parameter
point and the processes involved must be specified. Details on the dark showering are also included.

The points to be analyzed are to be inputed in Grid.dat.

Results are stored in the Results folder.

This code assumes the Feynrules model file has been put in the MadGraph model folder. It also assumed Delphes and Pythia have been installed and contain the proper files.
