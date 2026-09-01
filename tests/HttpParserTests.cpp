#include <gtest/gtest.h>

#include "HttpParser.h"

TEST(HttpParserTest, ParsesGetRequest)
{
    std::string request =
        "GET /hello HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "\r\n";

    HttpRequest parsed =
        HttpParser::parse(request);

    EXPECT_TRUE(parsed.valid);

    EXPECT_EQ(parsed.method, "GET");
    EXPECT_EQ(parsed.path, "/hello");
    EXPECT_EQ(parsed.version, "HTTP/1.1");
}

TEST(HttpParserTest, ParsesPostRequestWithBody)
{
    std::string request =
        "POST /users HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 6\r\n"
        "\r\n"
        "Masaod";

    HttpRequest parsed =
        HttpParser::parse(request);

    EXPECT_TRUE(parsed.valid);

    EXPECT_EQ(parsed.method, "POST");
    EXPECT_EQ(parsed.path, "/users");
    EXPECT_EQ(parsed.contentLength, 6);
    EXPECT_EQ(parsed.body, "Masaod");
}