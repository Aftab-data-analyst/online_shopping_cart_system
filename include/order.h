#ifndef ORDER_H
#define ORDER_H

#include "models.h"
#include <stdbool.h>

const char* getOrderStatusString(OrderStatus status);
bool processCheckout(Cart *cart, User *user, Product products[], int productCount, Order orders[], int *orderCount, int maxOrders);
void printInvoice(const Order *order);
void viewUserOrderHistory(const Order orders[], int orderCount, int userId);
void viewAllOrders(const Order orders[], int orderCount);
bool updateOrderStatus(Order orders[], int orderCount, int orderId, OrderStatus newStatus);
Order* findOrderById(Order orders[], int orderCount, int orderId);

#endif // ORDER_H

