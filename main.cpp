#include "inc/tcp_server.h"

int main() {
    printf("Boot\n");
    http::TCPServer server = http::TCPServer("127.0.0.1", 8080);

    return 0;
}