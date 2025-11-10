# Ì∑† C++ HTTP Server Learning Journey

From low-level system calls to a multithreaded HTTP server ‚Äî this repo documents my learning journey in **system-level C++**.

---

## Ì∫Ä Stages Overview

| Stage | Focus | Description |
|--------|--------|-------------|
| 1Ô∏è‚É£ | **C++ System-Level I/O** | Stack vs heap, file I/O, system calls (`open`, `read`, `write`, `close`) |
| 2Ô∏è‚É£ | **Networking (TCP/UDP)** | Sockets, IP, ports, client-server communication |
| 3Ô∏è‚É£ | **HTTP Protocol** | Understanding requests, responses, and headers |
| 4Ô∏è‚É£ | **Multithreading** | Handling multiple connections with `std::thread` and `std::mutex` |
| 5Ô∏è‚É£ | **Final HTTP Server** | Combining everything into a real HTTP server |

---

## Ì∑± Build & Run

```bash
cmake -S . -B build
cmake --build build -j
./build/bin/copy_file_syscalls

