#include <gtest/gtest.h>

#include "UserStore.h"

TEST(UserStoreTest, CreatesAndGetsUser)
{
    UserStore store;

    int id = store.createUser("Masaod");

    EXPECT_EQ(id, 1);

    std::string name;

    bool found = store.getUser(id, name);

    EXPECT_TRUE(found);
    EXPECT_EQ(name, "Masaod");
}

TEST(UserStoreTest, DeletesUser)
{
    UserStore store;

    int id = store.createUser("Masaod");

    EXPECT_TRUE(store.deleteUser(id));

    std::string name;

    EXPECT_FALSE(store.getUser(id, name));
}

TEST(UserStoreTest, ReturnsFalseForUnknownUser)
{
    UserStore store;

    std::string name;

    EXPECT_FALSE(store.getUser(100, name));
}