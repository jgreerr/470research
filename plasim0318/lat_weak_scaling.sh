#!/bin/bash

gcc nogui.c -o nogui.x

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the program:"

    ./nogui.x -n 1 -e 1_core_24_lat -t 0
    cd plasim/run
    source most_plasim_run
    cd ..
    cd ..

    ./nogui.x -n 2 -e 2_core_24_lat -t 0
    cd plasim/run
    source most_plasim_run
    cd ..
    cd ..

    ./nogui.x -n 4 -e 4_core_32_lat -t 1
    cd plasim/run 
    source most_plasim_run
    cd ..
    cd ..

    ./nogui.x -n 8 -e 8_core_48_lat -t 2
    cd plasim/run 
    source most_plasim_run
    cd ..
    cd ..

    ./nogui.x -n 16 -e 16_core_48_lat -t 2
    cd plasim/run 
    source most_plasim_run
    cd ..
    cd ..

    ./nogui.x -n 32 -e 32_core_64_lat -t 3
    cd plasim/run 
    source most_plasim_run
    cd ..
    cd ..

    ./nogui.x -n 64 -e 64_core_64_lat -t 3
    cd plasim/run 
    source most_plasim_run
    cd ..
    cd ..


else
    echo "Compilation failed."
fi