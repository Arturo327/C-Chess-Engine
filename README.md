# C Chess Engine

![C](https://img.shields.io/badge/language-C-00599C?logo=c&logoColor=white)
![Status](https://img.shields.io/badge/status-in%20development-yellow)

A lightweight, from-scratch chess engine implemented in C, designed for educational purposes and experimentation with fancy modern techniques.

---

## Features
- Pure C implementation (no frameworks)
- Alpha-beta pruning search
- Quiescence search to reduce horizon effect
- Bitboard representation
- Move ordering with SEE
- Transposition tables (Zobrist hashing)
- UCI protocol support
- Iterative deepening
- Killer moves
- History heuristic
- Principal variation search (PVS)
- Null move pruning
- Aspiration windows
- Late moves reduction (LMR)
- Delta pruning in quiescence search
- Threefold repetition detection
- Magic Bitboards for move generation
- Time control

---

## Why this project?

This project was built to:
- Understand how modern chess engines work and how they are capable of beating humans.
- Experiment with different evaluation and move ordering techniques.

---

## Project structure

```
c-chess-engine/
├── src/           # C source code
├── include/       # Header files
└── build/         # Compiled binaries

```

---

## Quick Start

Compile:
```bash
make
```

Then download Cute Chess (or something similar). 

tools -> settings -> engines -> add

Path the executable.

Enjoy!

---

## Performance

According to a test in which the engine faced Stockfish (limited to an Elo rating of 2500), and in which 50 games were played, the engine’s Elo rating is 2430.

---

## Future work

- Multi-threading
- Improve evaluation: pawn structure, mobility, center control...

---













