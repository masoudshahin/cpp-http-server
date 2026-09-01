# C++ Multithreaded HTTP Server

A multithreaded HTTP/1.1 server built from scratch in C++17 using POSIX sockets, a custom thread pool, routing, thread-safe in-memory storage, and automated tests.

## Features

- TCP server using POSIX sockets
- HTTP/1.1 request parsing
- GET, POST, and DELETE support
- Header, `Content-Length`, and request-body parsing
- Routing by HTTP method and path
- Dynamic routes such as `/users/{id}`
- Custom fixed-size thread pool
- Thread-safe in-memory `UserStore`
- HTTP response generation with status codes
- GoogleTest unit tests
- CMake build system

## Architecture

```text
Client
  |
  v
Server::accept()
  |
  v
ThreadPool
  |
  v
Server::handleClient()
  |
  v
HttpParser
  |
  v
Router ---------> UserStore
  |
  v
HttpResponse
  |
  v
Server::sendAll()
  |
  v
Client
```

`UserStore` is accessed only by routes that require user data.

## Tech Stack

| Layer | Technologies |
|---|---|
| Language | C++17 |
| Networking | POSIX Sockets / TCP |
| Concurrency | `std::thread`, `std::mutex`, `std::condition_variable` |
| Storage | `std::unordered_map` |
| Testing | GoogleTest |
| Build | CMake |
| Platform | macOS / Linux |

## Key Design Decisions

- **Fixed-size thread pool:** avoids creating a new thread for every incoming connection.
- **Thread-safe shared storage:** `UserStore` protects shared data with a mutex.
- **Separated responsibilities:** networking, parsing, routing, storage, and response generation are handled by separate components.
- **Complete request reading:** the server reads headers first, uses `Content-Length`, and continues receiving until the request body is complete.
- **Reliable response sending:** `sendAll()` continues until the full HTTP response has been transmitted.

## API Endpoints

| Method | Endpoint | Description |
|---|---|---|
| GET | `/` | Home endpoint |
| GET | `/hello` | Simple test endpoint |
| GET | `/about` | About endpoint |
| GET | `/users` | Get all users |
| POST | `/users` | Create a user from the request body |
| GET | `/users/{id}` | Get a user by ID |
| DELETE | `/users/{id}` | Delete a user by ID |

Main response codes include `200 OK`, `201 Created`, `400 Bad Request`, `404 Not Found`, and `405 Method Not Allowed`.

## Build & Run

```bash
git clone <repository-url>
cd cpp-http-server

mkdir build
cd build

cmake ..
cmake --build .

./server
```

The server runs at:

```text
http://localhost:8080
```

## API Examples

### 1. Hello Endpoint

```bash
curl -i http://localhost:8080/hello
```

Example response:

```text
HTTP/1.1 200 OK

Hello from server!
```

### 2. Add a User

Send a `POST` request to `/users`. The request body contains the user's name.

```bash
curl -i -X POST -d "Masaod" http://localhost:8080/users
```

Example response:

```text
HTTP/1.1 201 Created

Created user with ID: 1
```

Add another user:

```bash
curl -i -X POST -d "Ali" http://localhost:8080/users
```

Example response:

```text
HTTP/1.1 201 Created

Created user with ID: 2
```

### 3. Show All Users

```bash
curl -i http://localhost:8080/users
```

Example response:

```text
HTTP/1.1 200 OK

1: Masaod
2: Ali
```

If no users exist:

```text
HTTP/1.1 200 OK

No users
```

### 4. Show a Specific User

To get user `1`:

```bash
curl -i http://localhost:8080/users/1
```

Example response:

```text
HTTP/1.1 200 OK

ID: 1
Name: Masaod
```

To get user `2`:

```bash
curl -i http://localhost:8080/users/2
```

If the user does not exist:

```bash
curl -i http://localhost:8080/users/100
```

Example response:

```text
HTTP/1.1 404 Not Found

User not found
```

### 5. Delete a User

Delete user `1`:

```bash
curl -i -X DELETE http://localhost:8080/users/1
```

Example response:

```text
HTTP/1.1 200 OK

User deleted
```

Verify that the user was deleted:

```bash
curl -i http://localhost:8080/users/1
```

Example response:

```text
HTTP/1.1 404 Not Found

User not found
```

Show the remaining users:

```bash
curl -i http://localhost:8080/users
```

Example:

```text
2: Ali
```

### 6. Unsupported Method

For example, `/hello` only accepts `GET`:

```bash
curl -i -X POST http://localhost:8080/hello
```

Example response:

```text
HTTP/1.1 405 Method Not Allowed

Method not allowed
```

### 7. Unknown Route

```bash
curl -i http://localhost:8080/notfound
```

Example response:

```text
HTTP/1.1 404 Not Found

Page not found
```

## Testing

The project uses GoogleTest for automated unit testing.

```bash
cd build
cmake ..
cmake --build .
ctest --output-on-failure
```

Tests cover:

- HTTP request parsing
- GET and POST parsing
- Request-body parsing
- User creation, lookup, and deletion
- Router status codes
- Unknown routes
- Unsupported HTTP methods

## Project Structure

```text
cpp-http-server/
|-- CMakeLists.txt
|-- README.md
|-- include/
|   |-- Server.h
|   |-- HttpParser.h
|   |-- HttpResponse.h
|   |-- Router.h
|   |-- ThreadPool.h
|   `-- UserStore.h
|-- src/
|   `-- main.cpp
`-- tests/
    |-- HttpParserTests.cpp
    |-- UserStoreTests.cpp
    `-- RouterTests.cpp
```

## Author

**Masaod Shahin**  
Computer Science Student - Technion - Israel Institute of Technology  
Focus: C++ · Systems Programming · Backend Development  
GitHub: `masoudshahin`
