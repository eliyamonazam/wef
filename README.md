# Sudoku Game (C++)

A terminal-based Sudoku game implemented in C++. It supports multiple features like game saving/loading, leaderboard tracking, undo functionality, and various difficulty levels.

## Features

- **Sudoku Grid**: 9x9 grid where you can fill numbers from 1 to 9.
- **Difficulty Levels**: Choose from 3 levels (Easy, Medium, Hard).
- **Undo Functionality**: Undo the last move made during gameplay.
- **Leaderboard**: Tracks high scores with player name, score, and time.
- **Game Saving/Loading**: Save and load the current game state.
- **Timer**: Each game has a time limit based on difficulty.
- **Mistakes Tracking**: Up to 3 mistakes allowed before the game ends.
- **Pause Menu**: Pause the game and resume or save progress.

## Installation

1. Clone this repository:
    ```bash
    git clone https://github.com/nolimiya/sudoku-game.git
    cd sudoku-game
    ```

2. Compile the project using a C++ compiler (e.g., g++):
    ```bash
    g++ -o sudoku_game sudoku_game.cpp
    ```

3. Run the game:
    ```bash
    ./sudoku_game
    ```

## Usage

When the game starts, you will be prompted with several options:

1. **New Game**: Start a fresh game.
2. **Load Game**: Load a previously saved game.
3. **Leaderboard**: View the top 10 players based on score and time.
4. **Exit**: Quit the game.

During the game, use the following controls:

- **Arrow Keys**: Move the cursor around the grid.
- **Numbers (1-9)**: Input a number into the selected cell.
- **U**: Undo the last move.
- **S**: Save the current game.
- **Q**: Quit the game.
- **ESC**: Pause the game and open the pause menu.

### Timer

- Each level has a different time limit:
  - Easy: 5 minutes
  - Medium: 3 minutes
  - Hard: 2 minutes
- If the timer runs out, the game is over.

### Leaderboard

The leaderboard is updated at the end of each game and is saved in a file named `leaderboard.txt`. It stores the player's name, score, and time.

### Save and Load Game

You can save your game progress at any time. To load a saved game, select the "Load Game" option from the main menu. The game will restore the previous state, including the board, timer, and player information.

## Game Logic

The game generates a random Sudoku puzzle based on the difficulty level chosen. It ensures that the puzzle has a unique solution and is solvable. Players fill in the grid while making sure the numbers do not violate Sudoku rules.

The game ends when:
- The player completes the puzzle correctly.
- The player runs out of time.
- The player makes 3 mistakes.

## Acknowledgements

- [Sudoku Game Algorithm](https://en.wikipedia.org/wiki/Sudoku_solving_algorithms) - used as a reference for Sudoku puzzle generation.
