#pragma once

#include <iostream>
#include <vector>
#include <WinSock2.h>

namespace http {

#define BUFFERLEN 30720

    class TCPServer {
    public:
        // Constructor and destructor
        TCPServer(const char *ipParam, uint16_t portParam);
        ~TCPServer();

        // Read from server and send data to server
        bool readServer();
        bool sendToServer(const char *msg);
    private:
        const char *ip;
        const uint16_t port;

        WSADATA lpwsaData;
        SOCKET serverSocket;
        sockaddr_in socketAddress;
        // Clients that connect to HTTP server
        std::vector<SOCKET> clients;

        long incomingMsg;
        int socketAddresslength;

        // Server operation functions
        bool startServer();
        bool stopServer();
        bool startListening();
        bool acceptConnection(SOCKET &socket);

        // Log functions
        void log(const char *msg);
        void exitWithError(const char *msg);
    };
    
}