# Stage 1 â€” C++ System-Level I/O (Final)

## í¾¯ Goal
Combine all key concepts:
- Stack vs Heap (conceptually)
- File I/O using `fstream`
- Exception Handling & RAII
- Multithreading (`std::thread`)
- Synchronization (`std::mutex`)

---

## í·© Mini Project: Multithreaded Logger

This program:
1. Launches 3 threads writing logs to a shared file (`log.txt`)
2. Uses a mutex for safe file access
3. Reads the file afterward
4. Demonstrates exception safety

---

## í·± Build & Run

```bash
cmake -S ../.. -B ../../build
cmake --build ../../build -j
./../../build/bin/stage1_final

All threads finished.
Thread 1 wrote log 1
Thread 1 wrote log 2
Thread 2 wrote log 1
Thread 3 wrote log 1
...
