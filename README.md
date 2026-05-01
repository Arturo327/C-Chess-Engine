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
- Move ordering
- Transposition tables (Zobrist hashing)
- UCI protocol suport
- Iterative deepening
- Killer moves
- History heuristic
- Principal variation search (PVS)

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

Then download cutechess (or something similar). 

tools -> settings -> engines -> add

Enter a name and the address of the compiled engine.

Enjoy!

---

## Performance

Search time depends on position complexity, depth, and move ordering efficiency.

At depth 10, search time typically ranges from 1s to 20s in simple positions (final and beggining), and even 2 min at very complex positions.

According to Stockfish 18, it plays around 90% accurracy. It doesn’t mean much (I suck at chess), but the bot beats me easily.

Many times the games finish with a draw becouse position repetition.

---

## Future work

- Multi-threading
- Null move purning
- Improve evaluation: pawn stucture, mobility, differentiate game phases, center control...

---













