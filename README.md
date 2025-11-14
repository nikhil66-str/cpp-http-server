#  C++ HTTP Server Learning Journey

From low-level system calls to a multithreaded HTTP server — this repo documents my learning journey in **system-level C++**.

---

##  Stages Overview

| Stage | Focus | Description |
|--------|--------|-------------|
| 1️⃣ | **C++ System-Level I/O** | Stack vs heap, file I/O, system calls (`open`, `read`, `write`, `close`) |
| 2️⃣ | **Networking (TCP/UDP)** | Sockets, IP, ports, client-server communication |
| 3️⃣ | **HTTP Protocol** | Understanding requests, responses, and headers |
| 4️⃣ | **Multithreading** | Handling multiple connections with `std::thread` and `std::mutex` |
| 5️⃣ | **Final HTTP Server** | Combining everything into a real HTTP server |

---

##  Build & Run

```bash
cmake -S . -B build
cmake --build build -j
./build/bin/copy_file_syscalls

