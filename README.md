# Board Game Engine with Command Interpreter

> Academic project developed for **Laboratórios de Informática II** @ University of Minho  
> Final grade: **19/20** ⭐

## About

This project implements a board game engine in **C**, entirely through a command-line interface, no graphical interface. The game consists of a grid of letters where the player must mark cells as white or crossed out according to a set of logical rules.

The main challenge was building a robust architecture capable of handling game state, undo/redo mechanics, automated inference and a backtracking solver all in pure C with dynamic memory allocation!

## Features

- 📂 **Load/Save** - read and write game state from/to files
- ⬜ **Paint white / Cross out** - interact with individual cells
- ↩️ **Undo** - revert any previous action via a stack-based history
- ✅ **Verify** - check for rule violations (duplicates, adjacent crossed cells, isolated cells, connectivity)
- 💡 **Hint (a)** - apply one round of logical inference
- 💡 **Hint (A)** - apply inference repeatedly until the board stabilises
- 🤖 **Auto-solve (R)** - solve the puzzle automatically using inference + backtracking
- 🧪 **Unit tests** - test suite written with CUnit

## File Structure

```
board-game-engine/
├── jogo.h        # Data structures and function declarations
├── jogo.c        # Full game logic implementation
├── main.c        # CLI interpreter and command loop
├── testes.c      # CUnit test suite
├── Makefile      # Build configuration
└── t.txt         # Sample board (5x5)
```

## How to Run

```bash
# Compile
make

# Run
./jogo
```

## Commands

| Command | Description |
|---|---|
| `l file.txt` | Load game from file |
| `g file.txt` | Save game to file |
| `b a1` | Paint cell white |
| `r a1` | Cross out cell |
| `v` | Verify game state |
| `a` | Apply one inference pass |
| `A` | Apply inference until stable |
| `R` | Auto-solve the puzzle |
| `d` | Undo last action |
| `s` | Quit |

## Tech Stack

C · Dynamic Memory Allocation · Stack (undo/redo) · BFS · Backtracking · CUnit

## Authors

Carolina Dias — [@carolinavdias](https://github.com/carolinavdias)  
João Ribeiro 
Tiago Santos

BSc Computer Engineering · University of Minho
