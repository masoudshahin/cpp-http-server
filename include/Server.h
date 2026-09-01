#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <thread>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "HttpParser.h"
#include "HttpResponse.h"
#include "Router.h"
#include "ThreadPool.h"
#include "UserStore.h"

class Server
{
private:
    int port;
    int server_socket;

    ThreadPool thread_pool;
    UserStore user_store;

    static constexpr size_t MAX_REQUEST_SIZE =
        1024 * 1024;

    bool readHttpRequest(
        int client_socket,
        std::string &request)
    {
        char buffer[4096];

        size_t header_end =
            std::string::npos;

        while (header_end == std::string::npos)
        {
            int bytes_received = recv(
                client_socket,
                buffer,
                sizeof(buffer),
                0);

            if (bytes_received <= 0)
            {
                return false;
            }

            request.append(
                buffer,
                bytes_received);

            if (request.size() > MAX_REQUEST_SIZE)
            {
                return false;
            }

            header_end =
                request.find("\r\n\r\n");
        }

        size_t body_start =
            header_end + 4;

        HttpRequest partial_request =
            HttpParser::parse(request);

        if (!partial_request.valid)
        {
            return true;
        }

        size_t content_length =
            partial_request.contentLength;

        while (
            request.size() - body_start
            < content_length)
        {
            int bytes_received = recv(
                client_socket,
                buffer,
                sizeof(buffer),
                0);

            if (bytes_received <= 0)
            {
                return false;
            }

            request.append(
                buffer,
                bytes_received);

            if (request.size() > MAX_REQUEST_SIZE)
            {
                return false;
            }
        }

        size_t expected_size =
            body_start + content_length;

        if (request.size() > expected_size)
        {
            request.resize(expected_size);
        }

        return true;
    }

    bool sendAll(
        int client_socket,
        const std::string &data)
    {
        size_t total_sent = 0;

        while (total_sent < data.size())
        {
            ssize_t sent = send(
                client_socket,
                data.data() + total_sent,
                data.size() - total_sent,
                0);

            if (sent <= 0)
            {
                return false;
            }

            total_sent += sent;
        }

        return true;
    }

    void handleClient(int client_socket)
    {
        std::string raw_request;

        if (!readHttpRequest(
                client_socket,
                raw_request))
        {
            close(client_socket);
            return;
        }

        HttpRequest request =
            HttpParser::parse(raw_request);

        if (!request.valid)
        {
            HttpResponse bad_request(
                400,
                "Bad Request",
                "Invalid HTTP request");

            std::string response =
                bad_request.toString();

            sendAll(
                client_socket,
                response);

            close(client_socket);

            return;
        }

        HttpResponse http_response =
            Router::route(
                request,
                user_store);

        std::cout
            << "["
            << request.method
            << "] "
            << request.path
            << " -> "
            << http_response.getStatusCode()
            << std::endl;

        std::string response =
            http_response.toString();

        sendAll(
            client_socket,
            response);

        close(client_socket);
    }

    static size_t getThreadCount()
    {
        size_t count =
            std::thread::hardware_concurrency();

        if (count == 0)
        {
            return 4;
        }

        return count;
    }

public:
    Server(int port)
        : port(port),
          server_socket(-1),
          thread_pool(getThreadCount())
    {
    }

    void start()
    {
        server_socket =
            socket(
                AF_INET,
                SOCK_STREAM,
                0);

        if (server_socket == -1)
        {
            std::cerr
                << "Failed to create socket"
                << std::endl;

            return;
        }

        int reuse_address = 1;

        setsockopt(
            server_socket,
            SOL_SOCKET,
            SO_REUSEADDR,
            &reuse_address,
            sizeof(reuse_address));

        sockaddr_in server_address{};

        server_address.sin_family =
            AF_INET;

        server_address.sin_port =
            htons(port);

        server_address.sin_addr.s_addr =
            INADDR_ANY;

        if (bind(
                server_socket,
                reinterpret_cast<sockaddr *>(
                    &server_address),
                sizeof(server_address))
            == -1)
        {
            std::cerr
                << "Failed to bind socket"
                << std::endl;

            close(server_socket);

            return;
        }

        if (listen(
                server_socket,
                10)
            == -1)
        {
            std::cerr
                << "Failed to listen"
                << std::endl;

            close(server_socket);

            return;
        }

        std::cout
            << "Server is listening on port "
            << port
            << "..."
            << std::endl;

        while (true)
        {
            int client_socket =
                accept(
                    server_socket,
                    nullptr,
                    nullptr);

            if (client_socket == -1)
            {
                std::cerr
                    << "Failed to accept connection"
                    << std::endl;

                continue;
            }

            thread_pool.addTask(
                [this, client_socket]()
                {
                    handleClient(
                        client_socket);
                });
        }
    }
};

#endif