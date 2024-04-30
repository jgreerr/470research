#!/bin/bash

# Compile the C program
gcc -o nogui nogui.c

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running the program:"
    # Run the program
    ./nogui -n 1 -l 10 -y 1 -e Serial

    echo "Weak scaling"


else
    echo "Compilation failed."
fi