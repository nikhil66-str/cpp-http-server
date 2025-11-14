# Stage 1 — C++ System-Level I/O (Final)

## Goal
This stage focuses on mastering low-level system programming fundamentals in C++.  
Key concepts covered:

- Stack vs Heap memory (conceptual understanding)
- File I/O using `std::fstream`
- Exception handling and RAII
- Multithreading with `std::thread`
- Synchronization using `std::mutex`

---

## Mini Project: Multithreaded Logger

This final Stage 1 project demonstrates:

1. Launching 3 worker threads  
2. Each thread safely writing logs to a shared file (`log.txt`)  
3. File access protected with a mutex  
4. RAII and exception-safe design  
5. Reading back the file contents after all threads finish  

This project ties together everything learned in Stage 1.

---

## Build & Run

Using CMake (as configured in your repo):

```bash
cmake -S ../.. -B ../../build
cmake --build ../../build -j
./../../build/bin/stage1_final
