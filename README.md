# COA Assignment - Simulating a Simple Cache Controller FSM

This project simulates a cache controller that uses a Finite State Machine (FSM) to decide the necessary actions during memory read and write operations. The FSM handles states such as `IDLE`, `COMPARE_TAG`, `ALLOCATE`, and `WRITE_BACK` to emulate a realistic cache with misses and write-backs.

## Prerequisites
Ensure you have a C++ compiler installed (GCC/g++ or Clang).

- **Linux/Ubuntu:** `sudo apt install build-essential`
- **macOS:** `brew install gcc` or use Xcode Command Line Tools.
- **Windows:** MinGW or WSL (Windows Subsystem for Linux).

## Building the Project

To compile the source code into an executable, use the following command in your terminal:

```bash
g++ -Wall -std=c++17 TinySIM.cpp -o TinySIM
./TinySIM