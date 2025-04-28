#include "tcp_server.h"

namespace http {

    TCPServer::TCPServer(const char *ipParam, uint16_t portParam):
    ip(ipParam), port(portParam), lpwsaData(), serverSocket(), incomingMsg(), 
    socketAddress(), clients() ,socketAddresslength(sizeof(socketAddress)) {

        if(!startServer()) exitWithError("Starting HTTP server failed\n");
        else log("Starting HTTP server\n");

        if(!startListening()) exitWithError("Starting to listen socket failed\n");
        else log("Listening on ADDRESS: %i\nPORT:%i\n", inet_ntoa(socketAddress.sin_addr), ntohs(socketAddress.sin_port));

        //if(!acceptConnection()) exitWithError("Starting to accept connections failed\n");
    }

    TCPServer::~TCPServer() {
        if(!stopServer()) exitWithError("Can't stop HTTP server");
        else exit(0);
    }

    bool TCPServer::readServer() {
        char buffer[BUFFERLEN];
        long bytesReceived;
        char *msgBuffer;

        for(SOCKET &socket : clients) {
            // Read until no bytes received or error
            do {
                bytesReceived = recv(socket, buffer, strlen(buffer), 0);
                switch(bytesReceived) {
                    case SOCKET_ERROR:
                        return false;
                    case 0:
                        log("No bytes received\n");
                    default:
                        log("Received %i bytes\nReceived data: %s\n", bytesReceived, buffer);
                }
            } while(bytesReceived > 0);
        }

        return true;
    }

    bool TCPServer::acceptConnection(SOCKET &socket) {
        log("New connection arrived\n");
        socket = accept(serverSocket, (sockaddr *) &socketAddress, &socketAddresslength);
        if(socket == INVALID_SOCKET) return false;
        
        clients.emplace_back(socket);
        log("New connection accepted\nSending a response from the server\n");
        const char *msg = serverResponse();
        if(send(socket, msg, strlen(msg), 0) == SOCKET_ERROR) return false;

        return true;
    }

    bool TCPServer::sendToServer(const char *msg) {
        if(strlen(msg) > BUFFERLEN) return false;

        long bytesSent;
        if((bytesSent = send(serverSocket, msg, strlen(msg), 0)) == SOCKET_ERROR) return false;
        log("Sent %i bytes\n", bytesSent);

        return true;
    }

    void TCPServer::log(const char *format, ...) {
        if(strlen(format) > BUFFERLEN - 1) return;

        char msg[BUFFERLEN];
        va_list args;

        va_start(args, format);
        vsprintf(msg, format, args);
        va_end(args);

        printf("%s\n", msg);
    }

    void TCPServer::exitWithError(const char *msg) {
        /*std::string errMsg = "ERROR: ";
        errMsg += msg;
        if(errMsg[errMsg.size() - 1] != '\n') errMsg += "\n";
        errMsg += "CODE: ";
        errMsg += WSAGetLastError();*/
        
        log("ERROR: %s\nCODE: %i\n", msg, WSAGetLastError());
        exit(1);
    }

    bool TCPServer::startServer() {
        printf("Starting server\n");
        // Call WSAStartup to initialize use of the Winsock DLL by process with version 2.0
        if(WSAStartup(MAKEWORD(2, 0), &lpwsaData) != 0) return false;
        // Initialize TCP socket
        // Domain AF_INET = internet protocol 4 (IPv4) address family
        // Type SOCK_STREAM = socket provides sequalized two way connection-based byte streams with OOB data transmission
        // protcol 0 = no protocol specified, service providers protocol used
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if(serverSocket == INVALID_SOCKET) return false;

        // Initialize socket address and bind it to socket
        socketAddress.sin_family = AF_INET; // Socket family
        socketAddress.sin_port = htons(port); // Socket port stored in network byte order
        socketAddress.sin_addr.s_addr = inet_addr(ip); /*Socket IP address converted from 
        char * to unsigned long and stored in network byte order*/
        for(int i = 0, n = sizeof(socketAddress.sin_zero) / sizeof(socketAddress.sin_zero[0]); i < n; ++i) {
            // Initialize sin_zero array with zeros because we don't use it
            socketAddress.sin_zero[i] = 0;
        }
        if(bind(serverSocket, (sockaddr *) &socketAddress, socketAddresslength) == SOCKET_ERROR) return false;

        return true;
    }

    bool TCPServer::stopServer() {
        // Close socket and clean up WSA
        if(closesocket(serverSocket) != 0) return false;
        for(SOCKET &socket : clients) if(closesocket(socket) != 0) return false;
        if(WSACleanup() == SOCKET_ERROR) return false;

        return true;
    }

    bool TCPServer::startListening() {
        log("Starting listening\n");
        if(listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) return false;

        return true;
    }

    const char *TCPServer::serverResponse() const {
        std::string html = "<!DOCTYPE html><html lang=\"en\"><body><h1>HTTP server</h1></body></html>";
        std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-length: ";
        response += html.length();
        response += "\n\n";
        response += html;
        return response.c_str();
    }

}