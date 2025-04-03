#!/bin/bash

# Update the system
sudo apt-get update

# Install necessary dependencies
sudo apt-get install g++ make 

# Clone the repository (if not already cloned)
git clone https://github.com/eliyamonazam/sudoku-game.git
cd sudoku-game

# Compile the code
make

# Run the game
./sudoku_game
