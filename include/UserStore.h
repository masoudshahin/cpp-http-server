#ifndef USER_STORE_H
#define USER_STORE_H

#include <unordered_map>
#include <string>
#include <mutex>

class UserStore
{
private:
    std::unordered_map<int, std::string> users;

    std::mutex users_mutex;

    int next_id = 1;

public:
    int createUser(const std::string &name)
    {
        std::lock_guard<std::mutex> lock(users_mutex);

        int id = next_id++;

        users[id] = name;

        return id;
    }

    bool getUser(
        int id,
        std::string &name)
    {
        std::lock_guard<std::mutex> lock(users_mutex);

        auto it = users.find(id);

        if (it == users.end())
        {
            return false;
        }

        name = it->second;

        return true;
    }

    bool deleteUser(int id)
    {
        std::lock_guard<std::mutex> lock(users_mutex);

        return users.erase(id) > 0;
    }

    std::string getAllUsers()
    {
        std::lock_guard<std::mutex> lock(users_mutex);

        if (users.empty())
        {
            return "No users";
        }

        std::string result;

        for (const auto &user : users)
        {
            result +=
                std::to_string(user.first)
                + ": "
                + user.second
                + "\n";
        }

        return result;
    }
};

#endif