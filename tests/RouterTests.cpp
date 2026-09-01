#include <gtest/gtest.h>

#include "Router.h"
#include "UserStore.h"

TEST(RouterTest, HelloReturns200)
{
    UserStore store;

    HttpRequest request;

    request.method = "GET";
    request.path = "/hello";
    request.version = "HTTP/1.1";

    HttpResponse response =
        Router::route(request, store);

    EXPECT_EQ(response.getStatusCode(), 200);
}

TEST(RouterTest, UnknownPathReturns404)
{
    UserStore store;

    HttpRequest request;

    request.method = "GET";
    request.path = "/notfound";
    request.version = "HTTP/1.1";

    HttpResponse response =
        Router::route(request, store);

    EXPECT_EQ(response.getStatusCode(), 404);
}

TEST(RouterTest, PostHelloReturns405)
{
    UserStore store;

    HttpRequest request;

    request.method = "POST";
    request.path = "/hello";
    request.version = "HTTP/1.1";

    HttpResponse response =
        Router::route(request, store);

    EXPECT_EQ(response.getStatusCode(), 405);
}

TEST(RouterTest, CreatesUser)
{
    UserStore store;

    HttpRequest request;

    request.method = "POST";
    request.path = "/users";
    request.version = "HTTP/1.1";
    request.body = "Masaod";

    HttpResponse response =
        Router::route(request, store);

    EXPECT_EQ(response.getStatusCode(), 201);

    std::string name;

    EXPECT_TRUE(store.getUser(1, name));
    EXPECT_EQ(name, "Masaod");
}

TEST(RouterTest, DeletesUser)
{
    UserStore store;

    int id = store.createUser("Masaod");

    HttpRequest request;

    request.method = "DELETE";
    request.path =
        "/users/" + std::to_string(id);

    request.version = "HTTP/1.1";

    HttpResponse response =
        Router::route(request, store);

    EXPECT_EQ(response.getStatusCode(), 200);

    std::string name;

    EXPECT_FALSE(store.getUser(id, name));
}