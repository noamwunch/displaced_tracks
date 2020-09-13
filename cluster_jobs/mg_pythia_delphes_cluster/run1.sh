#!/bin/bash

# Dark eventst
cp ./Settings_dark.txt Settings.txt
cp ./Grid_dark.dat Grid.dat
#bash main_to_root.sh "dark1" $1
#bash main_to_root.sh "dark2" $1
#bash main_to_root.sh "dark3" $1
#bash main_to_root.sh "dark4" $1
#bash main_to_root.sh "dark5" $1
#bash main_to_root.sh "dark6" $1
#bash main_to_root.sh "dark7" $1
#bash main_to_root.sh "dark8" $1


# bb events
rm Settings.txt
cp Settings_bb.txt Settings.txt
cp ./Grid_bb.dat Grid.dat
./main_to_root.sh
#bash main_to_root.sh "bb2" $1
#bash main_to_root.sh "bb3" $1
#bash main_to_root.sh "bb4" $1
#bash main_to_root.sh "bb5" $1
#bash main_to_root.sh "bb6" $1
#bash main_to_root.sh "bb7" $1
#bash main_to_root.sh "bb8" $1

