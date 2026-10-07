#ifndef AUTH_H
#define AUTH_H

#include "models.h"
#include <stdbool.h>

User* authenticateUser(User users[], int userCount, const char *username, const char *password);
bool registerUser(User users[], int *userCount, int maxCapacity);
void displayUserProfile(const User *user);
bool updateUserProfile(User *user, User users[], int userCount);
User* findUserById(User users[], int userCount, int userId);

#endif // AUTH_H
