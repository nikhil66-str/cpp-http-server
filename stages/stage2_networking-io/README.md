# Stage 2 Final — Asynchronous Multi-Client IOCP Server (C++ / Windows)

This file represents the final combined implementation for Stage 2 of the C++ Networking Project. It merges the entire progression—from basic sockets to a fully asynchronous multi-client server—into a single, scalable IOCP-based architecture.

The server uses asynchronous `WSARecv`, a worker thread pool, and IOCP (I/O Completion Ports) to handle multiple clients efficiently without blocking or spawning a thread per client. This implementation forms the foundation for the HTTP server developed in Stage 3.

---

## Overview

This stage consolidates the following major concepts:

- Basic TCP socket programming  
- Thread-per-client model  
- Multi-client broadcast logic  
- select()-based I/O multiplexing  
- IOCP fundamentals  
- Asynchronous I/O using OVERLAPPED  
- Multi-client async event-driven server design  

The final result is an efficient architecture with a dedicated accept loop and a worker thread pool handling all I/O completions.

---

## Key Components

### 1. MyContext
A per-client state container holding:

- OVERLAPPED structure  
- WSABUF buffer  
- Message buffer  
- Socket handle  

Workers retrieve this context directly from IOCP completion packets.

---

### 2. IOCP Worker Thread Pool

A pool of worker threads is created based on CPU count.  
Each thread blocks on:

```
GetQueuedCompletionStatus
```

Workers handle:

- Completed asynchronous reads  
- Echoing messages back to clients  
- Re-posting `WSARecv`  
- Disconnect detection  
- Cleanup and resource release  

---

### 3. Asynchronous Read Pipeline

For each client:

1. Main thread posts initial `WSARecv`  
2. Client sends data  
3. IOCP queues a completion event  
4. Worker thread wakes and processes the buffer  
5. Worker re-posts the next `WSARecv`  
6. Loop continues until disconnect  

This pipeline avoids blocking entirely and scales across clients.

---

### 4. Main Thread Role

The main thread is responsible for:

- Accepting new TCP connections  
- Attaching sockets to IOCP  
- Allocating per-client context  
- Posting the initial asynchronous read  
- Returning to accept new clients  

All further operations are handled asynchronously by worker threads.

---

### 5. Graceful Shutdown Support

Exit packets are posted to IOCP using:

```
PostQueuedCompletionStatus(iocp, 0, 0, nullptr)
```

Workers exit cleanly, followed by handle cleanup.

---

## Build Instructions

### MinGW / g++

```
g++ stage2_final.cpp -o stage2_final -lws2_32
```

### MSVC

```
cl stage2_final.cpp /EHsc ws2_32.lib
```

---

## Running the Server

Start the server:

```
stage2_final.exe
```

Connect using Telnet or Netcat:

```
telnet localhost 8080
```

Type any message — the server echoes it asynchronously.  
Multiple clients are supported simultaneously.

---

## Features Implemented

- Fully asynchronous I/O using OVERLAPPED  
- Multi-client handling without thread-per-client  
- IOCP queue processing  
- Worker thread pool  
- Per-client context system  
- Continuous async receive → process → repost loop  
- Disconnect detection and cleanup  
- Scalable architecture similar to IIS-style servers  

---

## Limitations (Intentional for Learning)

- Uses synchronous `send()` (not `WSASend`)  
- Does not use AcceptEx  
- No HTTP parsing  
- No routing or static file serving  
- No Keep-Alive support  
- Logging is minimal  

These features will be added in Stage 3.

---



-




