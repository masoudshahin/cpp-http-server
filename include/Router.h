#ifndef ROUTER_H
#define ROUTER_H

#include <string>

#include "HttpParser.h"
#include "HttpResponse.h"
#include "UserStore.h"

class Router
{
private:
    static HttpResponse methodNotAllowed(
        const std::string &allowed_method)
    {
        HttpResponse response(
            405,
            "Method Not Allowed",
            "Method not allowed");

        response.setHeader(
            "Allow",
            allowed_method);

        return response;
    }

public:
    static HttpResponse route(
        const HttpRequest &request,
        UserStore &user_store)
    {
        if (request.path == "/")
        {
            if (request.method != "GET")
            {
                return methodNotAllowed("GET");
            }

            return HttpResponse(
                200,
                "OK",
                "Home page");
        }

        if (request.path == "/hello")
        {
            if (request.method != "GET")
            {
                return methodNotAllowed("GET");
            }

            return HttpResponse(
                200,
                "OK",
                "Hello from server!");
        }

        if (request.path == "/about")
        {
            if (request.method != "GET")
            {
                return methodNotAllowed("GET");
            }

            return HttpResponse(
                200,
                "OK",
                "About page");
        }

        // GET /users
        // POST /users

        if (request.path == "/users")
        {
            if (request.method == "GET")
            {
                return HttpResponse(
                    200,
                    "OK",
                    user_store.getAllUsers());
            }

            if (request.method == "POST")
            {
                if (request.body.empty())
                {
                    return HttpResponse(
                        400,
                        "Bad Request",
                        "User name is required");
                }

                int id =
                    user_store.createUser(
                        request.body);

                return HttpResponse(
                    201,
                    "Created",
                    "Created user with ID: "
                        + std::to_string(id));
            }

            HttpResponse response(
                405,
                "Method Not Allowed",
                "Method not allowed");

            response.setHeader(
                "Allow",
                "GET, POST");

            return response;
        }

        // /users/{id}

        const std::string prefix = "/users/";

        if (request.path.rfind(prefix, 0) == 0)
        {
            std::string id_string =
                request.path.substr(prefix.size());

            int id;

            try
            {
                id = std::stoi(id_string);
            }
            catch (...)
            {
                return HttpResponse(
                    400,
                    "Bad Request",
                    "Invalid user ID");
            }

            if (request.method == "GET")
            {
                std::string name;

                if (!user_store.getUser(id, name))
                {
                    return HttpResponse(
                        404,
                        "Not Found",
                        "User not found");
                }

                return HttpResponse(
                    200,
                    "OK",
                    "ID: "
                        + std::to_string(id)
                        + "\nName: "
                        + name);
            }

            if (request.method == "DELETE")
            {
                if (!user_store.deleteUser(id))
                {
                    return HttpResponse(
                        404,
                        "Not Found",
                        "User not found");
                }

                return HttpResponse(
                    200,
                    "OK",
                    "User deleted");
            }

            HttpResponse response(
                405,
                "Method Not Allowed",
                "Method not allowed");

            response.setHeader(
                "Allow",
                "GET, DELETE");

            return response;
        }

        return HttpResponse(
            404,
            "Not Found",
            "Page not found");
    }
};

#endif