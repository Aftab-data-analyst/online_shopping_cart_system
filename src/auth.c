#include "auth.h"
#include "file_io.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

User* authenticateUser(User users[], int userCount, const char *username, const char *password) {
    for (int i = 0; i < userCount; ++i) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            return &users[i];
        }
    }
    return NULL;
}

User* findUserById(User users[], int userCount, int userId) {
    for (int i = 0; i < userCount; ++i) {
        if (users[i].id == userId) {
            return &users[i];
        }
    }
    return NULL;
}

bool registerUser(User users[], int *userCount, int maxCapacity) {
    if (*userCount >= maxCapacity) {
        printf(COLOR_RED "User registry is at maximum capacity!\n" COLOR_RESET);
        return false;
    }

    clearScreen();
    printBanner("CUSTOMER REGISTRATION");

    char username[MAX_USERNAME_LEN];
    while (1) {
        getStringInput("Enter desired username: ", username, sizeof(username));
        bool exists = false;
        for (int i = 0; i < *userCount; ++i) {
            if (strcmp(users[i].username, username) == 0) {
                exists = true;
                break;
            }
        }
        if (exists) {
            printf(COLOR_RED "Username '%s' is already taken! Choose another.\n" COLOR_RESET, username);
        } else {
            break;
        }
    }

    char password[MAX_PASSWORD_LEN];
    getStringInput("Enter password (min 4 characters): ", password, sizeof(password));
    while (strlen(password) < 4) {
        printf(COLOR_RED "Password too short! Must be at least 4 characters.\n" COLOR_RESET);
        getStringInput("Enter password: ", password, sizeof(password));
    }

    char fullName[MAX_NAME_LEN];
    getStringInput("Enter full name: ", fullName, sizeof(fullName));

    char email[MAX_EMAIL_LEN];
    getStringInput("Enter email address: ", email, sizeof(email));

    char phone[MAX_PHONE_LEN];
    getStringInput("Enter phone number: ", phone, sizeof(phone));

    char address[MAX_ADDRESS_LEN];
    getStringInput("Enter default delivery address: ", address, sizeof(address));

    // Determine new user ID
    int nextId = 1;
    for (int i = 0; i < *userCount; ++i) {
        if (users[i].id >= nextId) {
            nextId = users[i].id + 1;
        }
    }

    User newUser;
    newUser.id = nextId;
    strncpy(newUser.username, username, MAX_USERNAME_LEN - 1);
    newUser.username[MAX_USERNAME_LEN - 1] = '\0';
    strncpy(newUser.password, password, MAX_PASSWORD_LEN - 1);
    newUser.password[MAX_PASSWORD_LEN - 1] = '\0';
    strncpy(newUser.fullName, fullName, MAX_NAME_LEN - 1);
    newUser.fullName[MAX_NAME_LEN - 1] = '\0';
    strncpy(newUser.email, email, MAX_EMAIL_LEN - 1);
    newUser.email[MAX_EMAIL_LEN - 1] = '\0';
    strncpy(newUser.phone, phone, MAX_PHONE_LEN - 1);
    newUser.phone[MAX_PHONE_LEN - 1] = '\0';
    strncpy(newUser.address, address, MAX_ADDRESS_LEN - 1);
    newUser.address[MAX_ADDRESS_LEN - 1] = '\0';
    newUser.role = ROLE_CUSTOMER;

    users[*userCount] = newUser;
    (*userCount)++;

    saveUsers(users, *userCount);

    printf(COLOR_GREEN "\nRegistration successful! Welcome, %s. You can now log in.\n" COLOR_RESET, fullName);
    pauseScreen();
    return true;
}

void displayUserProfile(const User *user) {
    if (!user) return;
    clearScreen();
    printBanner("USER ACCOUNT PROFILE");
    printf("  User ID       : %d\n", user->id);
    printf("  Username      : %s\n", user->username);
    printf("  Full Name     : %s\n", user->fullName);
    printf("  Email         : %s\n", user->email);
    printf("  Phone         : %s\n", user->phone);
    printf("  Address       : %s\n", user->address);
    printf("  Account Role  : %s\n", (user->role == ROLE_ADMIN) ? COLOR_RED "ADMINISTRATOR" COLOR_RESET : COLOR_CYAN "CUSTOMER" COLOR_RESET);
    printDivider('-', 70);
}

bool updateUserProfile(User *user, User users[], int userCount) {
    if (!user) return false;
    displayUserProfile(user);

    printf("\nUpdate Options:\n");
    printf("  [1] Update Full Name\n");
    printf("  [2] Update Email\n");
    printf("  [3] Update Phone Number\n");
    printf("  [4] Update Delivery Address\n");
    printf("  [5] Change Password\n");
    printf("  [0] Cancel\n");

    int choice = getIntInput("Select field to edit: ", 0, 5);
    if (choice == 0) return false;

    char buffer[128];
    switch (choice) {
        case 1:
            getStringInput("Enter new full name: ", buffer, sizeof(buffer));
            strncpy(user->fullName, buffer, MAX_NAME_LEN - 1);
            user->fullName[MAX_NAME_LEN - 1] = '\0';
            break;
        case 2:
            getStringInput("Enter new email address: ", buffer, sizeof(buffer));
            strncpy(user->email, buffer, MAX_EMAIL_LEN - 1);
            user->email[MAX_EMAIL_LEN - 1] = '\0';
            break;
        case 3:
            getStringInput("Enter new phone number: ", buffer, sizeof(buffer));
            strncpy(user->phone, buffer, MAX_PHONE_LEN - 1);
            user->phone[MAX_PHONE_LEN - 1] = '\0';
            break;
        case 4:
            getStringInput("Enter new delivery address: ", buffer, sizeof(buffer));
            strncpy(user->address, buffer, MAX_ADDRESS_LEN - 1);
            user->address[MAX_ADDRESS_LEN - 1] = '\0';
            break;
        case 5:
            getStringInput("Enter new password: ", buffer, sizeof(buffer));
            strncpy(user->password, buffer, MAX_PASSWORD_LEN - 1);
            user->password[MAX_PASSWORD_LEN - 1] = '\0';
            break;
    }

    saveUsers(users, userCount);
    printf(COLOR_GREEN "\nProfile updated successfully!\n" COLOR_RESET);
    pauseScreen();
    return true;
}

