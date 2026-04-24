# Ultra - UCI Chess Engine (C++)

A high-performance chess engine written in C++ implementing the Universal Chess Interface (UCI) protocol, designed for integration with external GUIs such as CuteChess.

---

## Overview

This project is a fully functional chess engine focused on efficient search, performance optimization, and modular engine design. It supports standard UCI communication, allowing it to be used with various chess interfaces and testing tools.

The UCI protocol enables seamless interaction between chess engines and GUIs by standardizing command-based communication via input/output streams


## Features

- Full UCI protocol support (compatible with GUI tools like CuteChess)
- Alpha-Beta pruning with optimized search tree exploration
- Transposition tables (hash-based position caching)
- Advanced move ordering for efficient pruning
- Time-controlled search
- Position analysis via FEN strings
- CLI-based engine execution


## How to Use

### Compile

```bash
g++ -O3 -std=c++17 -o engine main.cpp
```

### Using with GUI

Open a UCI-compatible GUI (e.g., CuteChess)

Add the compiled engine executable

Start a game or analyze positions
