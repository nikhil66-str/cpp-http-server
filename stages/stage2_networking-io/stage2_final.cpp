

#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>   
#include <windows.h>
#include <ws2tcpip.h>

#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>




struct MyContext {
    OVERLAPPED ov;
    WSABUF wsaBuf;
    char buffer[1024];
    SOCKET sock;
};

// Worker thread: waits on IOCP and handles completions
DWORD WINAPI WorkerThread(LPVOID param) {
    HANDLE iocp = (HANDLE)param;
    DWORD bytesTransferred;
    ULONG_PTR completionKey;
    LPOVERLAPPED overlapped;

    DWORD threadId = GetCurrentThreadId();
    std::cout << "[worker " << threadId << "] started\n";

    while (true) {
        BOOL ok = GetQueuedCompletionStatus(iocp, &bytesTransferred, &completionKey, &overlapped, INFINITE);

        if (!ok) {
            
            DWORD err = GetLastError();
            std::cerr << "[worker " << threadId << "] GetQueuedCompletionStatus error: " << err << "\n";
            
            continue;
        }

       
        if (completionKey == 0 && overlapped == nullptr) {
            std::cout << "[worker " << threadId << "] exit signal received\n";
            break;
        }

        
        MyContext* ctx = reinterpret_cast<MyContext*>(overlapped);

       
        if (bytesTransferred == 0) {
            std::cout << "[worker " << threadId << "] client disconnected (sock=" << ctx->sock << ")\n";
            closesocket(ctx->sock);
            delete ctx;
            continue;
        }

        
        size_t len = (bytesTransferred < (DWORD)sizeof(ctx->buffer)) ? bytesTransferred : (sizeof(ctx->buffer) - 1);
        ctx->buffer[len] = '\0';

        std::cout << "[worker " << threadId << "] recv from sock=" << ctx->sock
                  << " bytes=" << bytesTransferred << " msg=\"" << ctx->buffer << "\"\n";

       
        int sent = send(ctx->sock, ctx->buffer, (int)len, 0);
        if (sent == SOCKET_ERROR) {
            std::cerr << "[worker " << threadId << "] send failed: " << WSAGetLastError() << "\n";
            closesocket(ctx->sock);
            delete ctx;
            continue;
        }

        
        ZeroMemory(&ctx->ov, sizeof(OVERLAPPED));
        ctx->wsaBuf.buf = ctx->buffer;
        ctx->wsaBuf.len = sizeof(ctx->buffer);
        DWORD flags = 0;
        int res = WSARecv(ctx->sock, &ctx->wsaBuf, 1, NULL, &flags, &ctx->ov, NULL);
        if (res == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err != WSA_IO_PENDING) {
                std::cerr << "[worker " << threadId << "] WSARecv repost failed: " << err << "\n";
                closesocket(ctx->sock);
                delete ctx;
            }
        }
    }

    return 0;
}

int main() {
   
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET) {
        std::cerr << "socket() failed: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        std::cerr << "bind() failed: " << WSAGetLastError() << "\n";
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listen() failed: " << WSAGetLastError() << "\n";
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on 0.0.0.0:8080\n";

   
    HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if (iocp == NULL) {
        std::cerr << "CreateIoCompletionPort failed: " << GetLastError() << "\n";
        closesocket(listenSock);
        WSACleanup();
        return 1;
    }

   
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    int numWorkers = std::max(2, (int)sysInfo.dwNumberOfProcessors * 2);

    std::vector<HANDLE> workerThreads;
    for (int i = 0; i < numWorkers; ++i) {
        HANDLE h = CreateThread(NULL, 0, WorkerThread, iocp, 0, NULL);
        if (h == NULL) {
            std::cerr << "CreateThread failed: " << GetLastError() << "\n";
           
        } else {
            workerThreads.push_back(h);
        }
    }

   
    while (true) {
        sockaddr_in clientAddr{};
        int len = sizeof(clientAddr);
        SOCKET clientSock = accept(listenSock, (sockaddr*)&clientAddr, &len);
        if (clientSock == INVALID_SOCKET) {
            int err = WSAGetLastError();
            std::cerr << "accept() failed: " << err << "\n";
      
            continue;
        }

        std::cout << "Accepted client socket: " << clientSock << "\n";

       
        HANDLE h = CreateIoCompletionPort((HANDLE)clientSock, iocp, (ULONG_PTR)clientSock, 0);
        if (h == NULL) {
            std::cerr << "CreateIoCompletionPort attach failed: " << GetLastError() << "\n";
            closesocket(clientSock);
            continue;
        }

       
        MyContext* ctx = new MyContext();
        ZeroMemory(&ctx->ov, sizeof(OVERLAPPED));
        ctx->sock = clientSock;
        ctx->wsaBuf.buf = ctx->buffer;
        ctx->wsaBuf.len = sizeof(ctx->buffer);

       
        DWORD flags = 0;
        int rc = WSARecv(ctx->sock, &ctx->wsaBuf, 1, NULL, &flags, &ctx->ov, NULL);
        if (rc == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err != WSA_IO_PENDING) {
                std::cerr << "Initial WSARecv failed: " << err << "\n";
                closesocket(clientSock);
                delete ctx;
                continue;
            }
        }
        
    }

   
    for (size_t i = 0; i < workerThreads.size(); ++i) {
        PostQueuedCompletionStatus(iocp, 0, 0, nullptr);
    }
    WaitForMultipleObjects((DWORD)workerThreads.size(), workerThreads.data(), TRUE, INFINITE);
    for (HANDLE h : workerThreads) CloseHandle(h);
    CloseHandle(iocp);
    closesocket(listenSock);
    WSACleanup();
    return 0;
}
