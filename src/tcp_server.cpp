#include "tcp_server.h"

namespace http {

    TCPServer::TCPServer(const char *ipParam, uint16_t portParam):
    ip(ipParam), port(portParam), lpwsaData(), tcpSocket(), incomingMsg(), 
    socketAddress(), clients() ,socketAddresslength(sizeof(socketAddress)) {
        Errors err;
        if((err = startServer()) == success) log("Starting HTTP server\n");
        else exitWithError("Starting HTTP server failed\n");

        if((err = startListening()) != success) exitWithError("Starting to listen socket failed\n");

        if((err = acceptConnection()) != success) exitWithError("Starting to accept connections failed\n");
    }

    TCPServer::~TCPServer() {
        if(stopServer() == success) exit(0);
        else printf("Error %i occurred during stopping HTTP server\n", WSAGetLastError());
    }

    Errors TCPServer::readServer() {
        char buffer[BUFFERLEN];
        long bytesReceived;
        char *msgBuffer;

        // Read until no bytes received or error
        do {
            bytesReceived = recv(tcpSocket, buffer, strlen(buffer), 0);
            switch(bytesReceived) {
                case SOCKET_ERROR:
                    return err_read_request;
                case 0:
                    log("No bytes received\n");
                default:
                    sprintf(msgBuffer, "Received %i bytes\n", bytesReceived);
                    log(msgBuffer);
            }
        } while(bytesReceived > 0);

        return success;
    }

    Errors TCPServer::sendToServer(const char *msg) {
        if(strlen(msg) > BUFFERLEN) return err_buffer_overflow;

        long bytesSent;
        if((bytesSent = send(tcpSocket, msg, strlen(msg), 0)) == SOCKET_ERROR) return err_send_response;

        char *msgBuffer;
        sprintf(msgBuffer, "Sent %i bytes\n", bytesSent);
        log(msgBuffer);

        return success;
    }

    Errors TCPServer::startServer() {
        printf("Starting server\n");
        // Call WSAStartup to initialize use of the Winsock DLL by process with version 2.0
        if(WSAStartup(MAKEWORD(2, 0), &lpwsaData) != 0) return err_wsa_startup;
        // Initialize TCP socket
        // Domain AF_INET = internet protocol 4 (IPv4) address family
        // Type SOCK_STREAM = socket provides sequalized two way connection-based byte streams with OOB data transmission
        // protcol 0 = no protocol specified, service providers protocol used
        tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
        if(tcpSocket == INVALID_SOCKET) return err_create_socket;

        // Initialize socket address and bind it to socket
        socketAddress.sin_family = AF_INET; // Socket family
        socketAddress.sin_port = htons(port); // Socket port stored in network byte order
        socketAddress.sin_addr.s_addr = inet_addr(ip); /*Socket IP address converted from 
        char * to unsigned long and stored in network byte order*/
        for(int i = 0, n = sizeof(socketAddress.sin_zero) / sizeof(socketAddress.sin_zero[0]); i < n; ++i) {
            // Initialize sin_zero array with zeros because we don't use it
            socketAddress.sin_zero[i] = 0;
        }
        if(bind(tcpSocket, (sockaddr *) &socketAddress, socketAddresslength) == SOCKET_ERROR) return err_socket_bind;

        return success;
    }

    Errors TCPServer::stopServer() {
        // Close socket and clean up WSA
        if(closesocket(tcpSocket) != 0) return err_close_socket;
        
        if(WSACleanup() == SOCKET_ERROR) return err_wsa_cleanup;

        return success;
    }

    Errors TCPServer::startListening() {
        printf("Starting listening\n");
        if(listen(tcpSocket, SOMAXCONN) == SOCKET_ERROR) return err_start_listening;
        char *msgBuffer;
        sprintf(msgBuffer, "Listening on ADDRESS: %i\nPORT:%i\n", inet_ntoa(socketAddress.sin_addr), ntohs(socketAddress.sin_port));
        log(msgBuffer);

        return success;
    }

    Errors TCPServer::acceptConnection() {
        printf("Starting to accept connections\n");
        if(accept(tcpSocket, (sockaddr *) &socketAddress, &socketAddresslength) == INVALID_SOCKET) return err_socket_accept;
        log("Accepting connections to socket\n");

        return success;
    }

    void TCPServer::log(const char *msg) {
        printf("%s\n", msg);
    }

    void TCPServer::exitWithError(const char *msg) {
        std::string errMsg = "ERROR: ";
        errMsg += msg;
        
        log(errMsg.c_str());
        exit(WSAGetLastError());
    }

}