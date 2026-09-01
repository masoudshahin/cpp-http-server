#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <string>
#include <unordered_map>

class HttpResponse
{
private:
    int status_code;
    std::string status_text;

    std::unordered_map<std::string, std::string> headers;

    std::string body;

public:
    HttpResponse(
        int status_code,
        const std::string &status_text,
        const std::string &body)
        : status_code(status_code),
          status_text(status_text),
          body(body)
    {
        headers["Content-Type"] = "text/plain";
        headers["Connection"] = "close";
    }

    void setHeader(
        const std::string &name,
        const std::string &value)
    {
        headers[name] = value;
    }

    int getStatusCode() const
    {
        return status_code;
    }

    std::string toString() const
    {
        std::string response =
            "HTTP/1.1 "
            + std::to_string(status_code)
            + " "
            + status_text
            + "\r\n";

        for (const auto &header : headers)
        {
            response +=
                header.first
                + ": "
                + header.second
                + "\r\n";
        }

        response +=
            "Content-Length: "
            + std::to_string(body.size())
            + "\r\n";

        response += "\r\n";

        response += body;

        return response;
    }
};

#endif