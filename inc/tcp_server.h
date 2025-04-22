#pragma once

#include <iostream>
#include <vector>
#include <WinSock2.h>

namespace http {

#define BUFFERLEN 30720

    enum Errors {
        success = 0,
        err_wsa_startup = -1,
        err_create_socket = -2,
        err_socket_bind = -3,
        err_start_listening = -4,
        err_socket_accept = -5,
        err_read_request = -6,
        err_send_response = -7,
        err_close_socket = -8,
        err_wsa_cleanup = -9,
        err_buffer_overflow = -10
    };

    class TCPServer {
    public:
        // Constructor and destructor
        TCPServer(const char *ipParam, uint16_t portParam);
        ~TCPServer();

        // Read from server and send data to server
        Errors readServer();
        Errors sendToServer(const char *msg);
    private:
        const char *ip;
        const uint16_t port;

        WSADATA lpwsaData;
        SOCKET tcpSocket;
        sockaddr_in socketAddress;
        // Clients that connect to HTTP server
        std::vector<SOCKET> clients;

        long incomingMsg;
        int socketAddresslength;

        // Server operation functions
        Errors startServer();
        Errors stopServer();
        Errors startListening();
        Errors acceptConnection();

        // Log functions
        void log(const char *msg);
        void exitWithError(const char *msg);
    };
    
}