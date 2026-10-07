#ifndef ADMIN_H
#define ADMIN_H

#include <stdbool.h>
#include "models.h"

void adminDashboard(Product products[], int *productCount, int maxProducts,
                    User users[], int *userCount, int maxUsers,
                    Order orders[], int *orderCount, int maxOrders);
bool addProduct(Product products[], int *productCount, int maxProducts);
bool updateProduct(Product products[], int productCount);
bool deleteProduct(Product products[], int *productCount);
void viewLowStockReport(const Product products[], int count, int threshold);
void viewSalesReport(const Order orders[], int orderCount);

#endif // ADMIN_H
