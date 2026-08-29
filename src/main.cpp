#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <string>

void handle_client(int client_socket) {
    char buffer[1024];

    int bytes_received = recv(
        client_socket,
        buffer,
        sizeof(buffer) - 1,
        0
    );

    if (bytes_received <= 0) {
        close(client_socket);
        return;
    }

    buffer[bytes_received] = '\0';

    std::cout << "\nHTTP Request:\n";
    std::cout << buffer << std::endl;

    std::string request(buffer);
    std::istringstream request_stream(request);

    std::string method;
    std::string path;
    std::string version;

    request_stream >> method >> path >> version;

    std::cout << "Method: " << method << std::endl;
    std::cout << "Path: " << path << std::endl;
    std::cout << "Version: " << version << std::endl;

    std::string body;

    if (path == "/") {
        body = "Home page";
    }
    else if (path == "/hello") {
        body = "Hello from server!";
    }
    else if (path == "/about") {
        body = "About page";
    }
    else {
        body = "Page not found";
    }

    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        body;

    send(
        client_socket,
        response.c_str(),
        response.size(),
        0
    );

    close(client_socket);
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    sockaddr_in server_address{};

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(
            server_socket,
            reinterpret_cast<sockaddr*>(&server_address),
            sizeof(server_address)
        ) == -1) {

        std::cerr << "Failed to bind socket" << std::endl;
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, 5) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        close(server_socket);
        return 1;
    }

    std::cout << "Server is listening on port 8080..." << std::endl;

    while (true) {
        int client_socket = accept(
            server_socket,
            nullptr,
            nullptr
        );

        if (client_socket == -1) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        std::cout << "Client connected!" << std::endl;

        handle_client(client_socket);
    }

    close(server_socket);

    return 0;
}