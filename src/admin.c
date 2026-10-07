#include "admin.h"
#include "catalog.h"
#include "order.h"
#include "file_io.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

bool addProduct(Product products[], int *productCount, int maxProducts) {
    if (*productCount >= maxProducts) {
        printf(COLOR_RED "Catalog is at maximum capacity!\n" COLOR_RESET);
        return false;
    }

    clearScreen();
    printBanner("ADD NEW PRODUCT");

    int id = getIntInput("Enter unique Product ID (1-9999): ", 1, 9999);
    for (int i = 0; i < *productCount; ++i) {
        if (products[i].id == id) {
            printf(COLOR_RED "Error: Product ID %d already exists!\n" COLOR_RESET, id);
            pauseScreen();
            return false;
        }
    }

    Product p;
    p.id = id;
    getStringInput("Enter Product Name: ", p.name, sizeof(p.name));
    getStringInput("Enter Category (e.g. Electronics, Clothing, Books): ", p.category, sizeof(p.category));
    getStringInput("Enter Short Description: ", p.description, sizeof(p.description));
    p.price = getDoubleInput("Enter Unit Price ($0.01 - $100000.00): ", 0.01, 100000.00);
    p.stock = getIntInput("Enter Initial Stock Quantity (0 - 10000): ", 0, 10000);
    p.rating = getDoubleInput("Enter Product Rating (1.0 - 5.0): ", 1.0, 5.0);

    products[*productCount] = p;
    (*productCount)++;

    saveProducts(products, *productCount);
    printf(COLOR_GREEN "\nProduct '%s' (ID %d) added successfully!\n" COLOR_RESET, p.name, p.id);
    pauseScreen();
    return true;
}

bool updateProduct(Product products[], int productCount) {
    clearScreen();
    printBanner("UPDATE PRODUCT DETAILS");

    displayCatalog(products, productCount);
    int id = getIntInput("\nEnter Product ID to update (or 0 to cancel): ", 0, 9999);
    if (id == 0) return false;

    Product *p = findProductById(products, productCount, id);
    if (!p) {
        printf(COLOR_RED "Product with ID %d not found!\n" COLOR_RESET, id);
        pauseScreen();
        return false;
    }

    printf("\nCurrently editing: [%d] %s ($%.2f, Stock: %d)\n", p->id, p->name, p->price, p->stock);
    printf("  [1] Update Price\n");
    printf("  [2] Update Stock Quantity\n");
    printf("  [3] Update Name\n");
    printf("  [4] Update Category\n");
    printf("  [5] Update Description\n");
    printf("  [6] Update Rating\n");
    printf("  [0] Cancel\n");

    int choice = getIntInput("Select attribute to modify: ", 0, 6);
    switch (choice) {
        case 1:
            p->price = getDoubleInput("Enter new price ($): ", 0.01, 100000.00);
            break;
        case 2:
            p->stock = getIntInput("Enter new stock count: ", 0, 100000);
            break;
        case 3:
            getStringInput("Enter new name: ", p->name, sizeof(p->name));
            break;
        case 4:
            getStringInput("Enter new category: ", p->category, sizeof(p->category));
            break;
        case 5:
            getStringInput("Enter new description: ", p->description, sizeof(p->description));
            break;
        case 6:
            p->rating = getDoubleInput("Enter new rating (1.0 - 5.0): ", 1.0, 5.0);
            break;
        case 0:
            return false;
    }

    saveProducts(products, productCount);
    printf(COLOR_GREEN "\nProduct updated successfully!\n" COLOR_RESET);
    pauseScreen();
    return true;
}

bool deleteProduct(Product products[], int *productCount) {
    clearScreen();
    printBanner("REMOVE PRODUCT FROM CATALOG");

    displayCatalog(products, *productCount);
    int id = getIntInput("\nEnter Product ID to delete (or 0 to cancel): ", 0, 9999);
    if (id == 0) return false;

    int index = -1;
    for (int i = 0; i < *productCount; ++i) {
        if (products[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf(COLOR_RED "Product ID %d not found!\n" COLOR_RESET, id);
        pauseScreen();
        return false;
    }

    printf(COLOR_YELLOW "Are you sure you want to delete '%s' (ID %d)? [1] Yes  [2] No: " COLOR_RESET,
           products[index].name, products[index].id);
    int confirm = getIntInput("", 1, 2);
    if (confirm != 1) {
        printf("Deletion cancelled.\n");
        pauseScreen();
        return false;
    }

    for (int i = index; i < *productCount - 1; ++i) {
        products[i] = products[i + 1];
    }
    (*productCount)--;

    saveProducts(products, *productCount);
    printf(COLOR_GREEN "\nProduct deleted from catalog.\n" COLOR_RESET);
    pauseScreen();
    return true;
}

void viewLowStockReport(const Product products[], int count, int threshold) {
    clearScreen();
    char title[64];
    snprintf(title, sizeof(title), "INVENTORY REPORT - LOW STOCK ALERT (<= %d units)", threshold);
    printBanner(title);

    int lowCount = 0;
    for (int i = 0; i < count; ++i) {
        if (products[i].stock <= threshold) {
            if (lowCount == 0) {
                printf(COLOR_CYAN);
                printDivider('-', 97);
                printf("| %-4s | %-24s | %-12s | %-8s | %-5s | %-5s | %-28s |\n",
                       "ID", "Product Name", "Category", "Price($)", "Stock", "Rate*", "Description");
                printDivider('-', 97);
                printf(COLOR_RESET);
            }
            displayProductRow(&products[i]);
            lowCount++;
        }
    }

    if (lowCount == 0) {
        printf(COLOR_GREEN "All products have healthy inventory levels above %d units!\n" COLOR_RESET, threshold);
    } else {
        printDivider('-', 97);
        printf(COLOR_YELLOW "Total items requiring restock: %d\n" COLOR_RESET, lowCount);
    }
    pauseScreen();
}

void viewSalesReport(const Order orders[], int orderCount) {
    clearScreen();
    printBanner("STORE SALES & REVENUE REPORT");

    if (orderCount == 0) {
        printf(COLOR_YELLOW "No sales recorded yet.\n" COLOR_RESET);
        pauseScreen();
        return;
    }

    double totalRevenue = 0.0;
    double totalDiscounts = 0.0;
    double totalTax = 0.0;
    int itemsSold = 0;
    int statusCounts[5] = {0};

    for (int i = 0; i < orderCount; ++i) {
        totalRevenue += orders[i].totalAmount;
        totalDiscounts += orders[i].discount;
        totalTax += orders[i].tax;

        for (int j = 0; j < orders[i].itemCount; ++j) {
            itemsSold += orders[i].items[j].quantity;
        }

        if (orders[i].status >= 0 && orders[i].status <= 4) {
            statusCounts[orders[i].status]++;
        }
    }

    printf("  Total Orders Placed     : %d\n", orderCount);
    printf("  Total Items Sold        : %d units\n", itemsSold);
    printf("  Total Gross Revenue     : " COLOR_GREEN "$%.2f" COLOR_RESET "\n", totalRevenue);
    printf("  Average Order Value     : $%.2f\n", totalRevenue / orderCount);
    printf("  Total Customer Discounts: -$%.2f\n", totalDiscounts);
    printf("  Total Sales Tax Collected: $%.2f\n", totalTax);
    printDivider('-', 60);

    printf(COLOR_CYAN "Order Status Breakdown:\n" COLOR_RESET);
    printf("  - Pending Orders        : %d\n", statusCounts[ORDER_PENDING]);
    printf("  - Confirmed Orders      : %d\n", statusCounts[ORDER_CONFIRMED]);
    printf("  - Shipped Orders        : %d\n", statusCounts[ORDER_SHIPPED]);
    printf("  - Delivered Orders      : %d\n", statusCounts[ORDER_DELIVERED]);
    printf("  - Cancelled Orders      : %d\n", statusCounts[ORDER_CANCELLED]);
    printDivider('=', 60);

    pauseScreen();
}

void adminDashboard(Product products[], int *productCount, int maxProducts,
                    User users[], int *userCount, int maxUsers,
                    Order orders[], int *orderCount, int maxOrders) {
    (void)users;
    (void)userCount;
    (void)maxUsers;
    (void)maxOrders;

    while (1) {
        clearScreen();
        printBanner("ADMINISTRATOR CONTROL PANEL");
        printf("  [1] View Product Catalog\n");
        printf("  [2] Add New Product\n");
        printf("  [3] Update Existing Product\n");
        printf("  [4] Delete Product\n");
        printf("  [5] Low Stock Inventory Alert (< 10 units)\n");
        printf("  [6] View All Customer Orders\n");
        printf("  [7] Update Order Status\n");
        printf("  [8] View Sales & Revenue Summary\n");
        printf("  [0] Logout & Return to Main Menu\n");
        printDivider('-', 70);

        int choice = getIntInput("Select an option: ", 0, 8);
        switch (choice) {
            case 1:
                clearScreen();
                printBanner("CURRENT INVENTORY CATALOG");
                displayCatalog(products, *productCount);
                pauseScreen();
                break;
            case 2:
                addProduct(products, productCount, maxProducts);
                break;
            case 3:
                updateProduct(products, *productCount);
                break;
            case 4:
                deleteProduct(products, productCount);
                break;
            case 5:
                viewLowStockReport(products, *productCount, 10);
                break;
            case 6:
                viewAllOrders(orders, *orderCount);
                break;
            case 7: {
                clearScreen();
                printBanner("UPDATE ORDER STATUS");
                if (*orderCount == 0) {
                    printf(COLOR_YELLOW "No orders to update.\n" COLOR_RESET);
                    pauseScreen();
                    break;
                }
                int oid = getIntInput("Enter Order ID to update: ", 1, 999999);
                Order *ord = findOrderById(orders, *orderCount, oid);
                if (!ord) {
                    printf(COLOR_RED "Order ID #%d not found.\n" COLOR_RESET, oid);
                    pauseScreen();
                    break;
                }
                printf("Current Status: %s\n", getOrderStatusString(ord->status));
                printf("Select New Status:\n");
                printf("  [0] PENDING\n");
                printf("  [1] CONFIRMED\n");
                printf("  [2] SHIPPED\n");
                printf("  [3] DELIVERED\n");
                printf("  [4] CANCELLED\n");
                int newSt = getIntInput("Choice: ", 0, 4);
                updateOrderStatus(orders, *orderCount, oid, (OrderStatus)newSt);
                pauseScreen();
                break;
            }
            case 8:
                viewSalesReport(orders, *orderCount);
                break;
            case 0:
                printf(COLOR_GREEN "Logging out of Administrator Panel...\n" COLOR_RESET);
                return;
        }
    }
}
