#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <cctype>

struct HttpRequest
{
    std::string method;
    std::string path;
    std::string version;

    std::unordered_map<std::string, std::string> headers;

    std::string body;

    size_t contentLength = 0;

    bool valid = true;
};

class HttpParser
{
private:
    static std::string trim(const std::string &text)
    {
        size_t start = text.find_first_not_of(" \t");

        if (start == std::string::npos)
        {
            return "";
        }

        size_t end = text.find_last_not_of(" \t");

        return text.substr(
            start,
            end - start + 1);
    }

    static std::string toLower(std::string text)
    {
        std::transform(
            text.begin(),
            text.end(),
            text.begin(),
            [](unsigned char c)
            {
                return std::tolower(c);
            });

        return text;
    }

public:
    static HttpRequest parse(const std::string &request)
    {
        HttpRequest parsed_request;

        std::istringstream stream(request);

        std::string line;

        // First line:
        // GET /hello HTTP/1.1

        if (!std::getline(stream, line))
        {
            parsed_request.valid = false;
            return parsed_request;
        }

        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        std::istringstream first_line(line);

        if (!(first_line
              >> parsed_request.method
              >> parsed_request.path
              >> parsed_request.version))
        {
            parsed_request.valid = false;
            return parsed_request;
        }

        // Read headers

        while (std::getline(stream, line))
        {
            if (!line.empty() && line.back() == '\r')
            {
                line.pop_back();
            }

            // Empty line = end of headers
            if (line.empty())
            {
                break;
            }

            size_t colon_position = line.find(':');

            if (colon_position == std::string::npos)
            {
                parsed_request.valid = false;
                return parsed_request;
            }

            std::string name =
                line.substr(0, colon_position);

            std::string value =
                line.substr(colon_position + 1);

            name = toLower(trim(name));
            value = trim(value);

            parsed_request.headers[name] = value;
        }

        // Content-Length

        auto content_length_header =
            parsed_request.headers.find("content-length");

        if (content_length_header !=
            parsed_request.headers.end())
        {
            try
            {
                parsed_request.contentLength =
                    std::stoull(
                        content_length_header->second);
            }
            catch (...)
            {
                parsed_request.valid = false;
                return parsed_request;
            }
        }

        // Everything after the empty line is the body

        std::string body{
            std::istreambuf_iterator<char>(stream),
            std::istreambuf_iterator<char>()};

        parsed_request.body = body;

        return parsed_request;
    }
};

#endif