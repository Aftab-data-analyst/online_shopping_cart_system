#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdbool.h>
#include "models.h"

#define PRODUCTS_FILE "data/products.txt"
#define USERS_FILE    "data/users.txt"
#define ORDERS_FILE   "data/orders.txt"

void ensureDataDirectoryExists(void);

int loadProducts(Product products[], int maxCapacity);
bool saveProducts(const Product products[], int count);

int loadUsers(User users[], int maxCapacity);
bool saveUsers(const User users[], int count);

int loadOrders(Order orders[], int maxCapacity);
bool saveOrders(const Order orders[], int count);
bool appendOrder(const Order *order);

void seedInitialData(void);

#endif // FILE_IO_H
