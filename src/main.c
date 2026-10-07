#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "models.h"
#include "common.h"
#include "file_io.h"
#include "catalog.h"
#include "cart.h"
#include "auth.h"
#include "order.h"
#include "admin.h"

static void browseCatalogMenu(Product products[], int productCount, Cart *cart) {
    while (1) {
        clearScreen();
        printBanner("EXPLORE PRODUCTS");
        printf("  [1] View All Products (Paginated)\n");
        printf("  [2] Search Products by Name / Keyword\n");
        printf("  [3] Filter Products by Category\n");
        printf("  [4] Sort Products by Price (Lowest to Highest)\n");
        printf("  [5] Sort Products by Price (Highest to Lowest)\n");
        printf("  [6] Sort Products by Customer Rating\n");
        printf("  [7] Quick Add Product to Cart\n");
        printf("  [0] Return to Previous Menu\n");
        printDivider('-', 70);

        int choice = getIntInput("Select an option: ", 0, 7);
        if (choice == 0) break;

        switch (choice) {
            case 1:
                displayCatalogPaginated(products, productCount);
                break;
            case 2: {
                clearScreen();
                printBanner("SEARCH PRODUCTS");
                char keyword[64];
                getStringInput("Enter search keyword or product name: ", keyword, sizeof(keyword));
                searchProductsByName(products, productCount, keyword);
                pauseScreen();
                break;
            }
            case 3: {
                clearScreen();
                printBanner("FILTER BY CATEGORY");
                listCategories(products, productCount);
                char cat[32];
                getStringInput("\nEnter category name to filter: ", cat, sizeof(cat));
                filterProductsByCategory(products, productCount, cat);
                pauseScreen();
                break;
            }
            case 4: {
                // Work on a copy of products for sorting view
                Product tempProducts[MAX_PRODUCTS];
                memcpy(tempProducts, products, sizeof(Product) * productCount);
                sortProductsByPrice(tempProducts, productCount, true);
                clearScreen();
                printBanner("PRODUCTS SORTED BY PRICE (LOW TO HIGH)");
                displayCatalog(tempProducts, productCount);
                pauseScreen();
                break;
            }
            case 5: {
                Product tempProducts[MAX_PRODUCTS];
                memcpy(tempProducts, products, sizeof(Product) * productCount);
                sortProductsByPrice(tempProducts, productCount, false);
                clearScreen();
                printBanner("PRODUCTS SORTED BY PRICE (HIGH TO LOW)");
                displayCatalog(tempProducts, productCount);
                pauseScreen();
                break;
            }
            case 6: {
                Product tempProducts[MAX_PRODUCTS];
                memcpy(tempProducts, products, sizeof(Product) * productCount);
                sortProductsByRating(tempProducts, productCount);
                clearScreen();
                printBanner("PRODUCTS SORTED BY TOP RATING");
                displayCatalog(tempProducts, productCount);
                pauseScreen();
                break;
            }
            case 7: {
                if (!cart) {
                    printf(COLOR_YELLOW "Please log in to add items to your cart!\n" COLOR_RESET);
                    pauseScreen();
                    break;
                }
                clearScreen();
                printBanner("ADD ITEM TO CART");
                displayCatalog(products, productCount);
                int pid = getIntInput("\nEnter Product ID to add: ", 1, 9999);
                int qty = getIntInput("Enter Quantity to add: ", 1, 1000);
                addToCart(cart, products, productCount, pid, qty);
                pauseScreen();
                break;
            }
        }
    }
}

static void customerSession(User *currentUser, Product products[], int *productCount,
                            User users[], int userCount,
                            Order orders[], int *orderCount) {
    Cart userCart;
    initCart(&userCart);

    while (1) {
        clearScreen();
        char title[128];
        snprintf(title, sizeof(title), "CUSTOMER DASHBOARD - Welcome, %s! (Items in Cart: %d)",
                 currentUser->fullName, userCart.itemCount);
        printBanner(title);

        printf("  [1] Browse & Search Catalog\n");
        printf("  [2] View Current Shopping Cart\n");
        printf("  [3] Add Product to Cart\n");
        printf("  [4] Update Item Quantity in Cart\n");
        printf("  [5] Remove Item from Cart\n");
        printf("  [6] Apply Promo Coupon Code\n");
        printf("  [7] Clear Cart\n");
        printf("  [8] Proceed to Checkout & Pay\n");
        printf("  [9] View My Order History & Invoices\n");
        printf("  [10] View / Update My Profile\n");
        printf("  [0] Logout & Exit to Main Menu\n");
        printDivider('-', 70);

        int choice = getIntInput("Select an option: ", 0, 10);
        switch (choice) {
            case 1:
                browseCatalogMenu(products, *productCount, &userCart);
                break;
            case 2:
                clearScreen();
                printBanner("YOUR SHOPPING CART");
                displayCart(&userCart);
                pauseScreen();
                break;
            case 3: {
                clearScreen();
                printBanner("ADD TO CART");
                displayCatalog(products, *productCount);
                int pid = getIntInput("\nEnter Product ID: ", 1, 9999);
                int qty = getIntInput("Enter Quantity: ", 1, 100);
                addToCart(&userCart, products, *productCount, pid, qty);
                pauseScreen();
                break;
            }
            case 4: {
                clearScreen();
                printBanner("UPDATE CART QUANTITY");
                if (userCart.itemCount == 0) {
                    printf(COLOR_YELLOW "Your cart is empty.\n" COLOR_RESET);
                    pauseScreen();
                    break;
                }
                displayCart(&userCart);
                int pid = getIntInput("\nEnter Product ID to update: ", 1, 9999);
                int newQty = getIntInput("Enter new quantity (0 to remove): ", 0, 1000);
                updateCartQuantity(&userCart, products, *productCount, pid, newQty);
                pauseScreen();
                break;
            }
            case 5: {
                clearScreen();
                printBanner("REMOVE ITEM FROM CART");
                if (userCart.itemCount == 0) {
                    printf(COLOR_YELLOW "Your cart is empty.\n" COLOR_RESET);
                    pauseScreen();
                    break;
                }
                displayCart(&userCart);
                int pid = getIntInput("\nEnter Product ID to remove: ", 1, 9999);
                removeFromCart(&userCart, pid);
                pauseScreen();
                break;
            }
            case 6: {
                clearScreen();
                printBanner("APPLY PROMO COUPON");
                if (userCart.itemCount == 0) {
                    printf(COLOR_YELLOW "Add items to your cart before applying a coupon.\n" COLOR_RESET);
                    pauseScreen();
                    break;
                }
                printf(COLOR_CYAN "Available Promo Codes:\n" COLOR_RESET);
                printf("  SAVE10   - 10%% off total\n");
                printf("  SAVE20   - 20%% off total\n");
                printf("  BIGSALE  - 25%% off total\n");
                printf("  FLAT50   - $50 off on orders >= $150\n");
                printf("  FREESHIP - Free shipping on any order\n\n");
                char code[32];
                getStringInput("Enter coupon code: ", code, sizeof(code));
                applyCouponCode(&userCart, code);
                pauseScreen();
                break;
            }
            case 7:
                clearCart(&userCart);
                pauseScreen();
                break;
            case 8:
                processCheckout(&userCart, currentUser, products, *productCount, orders, orderCount, MAX_ORDERS);
                break;
            case 9:
                viewUserOrderHistory(orders, *orderCount, currentUser->id);
                break;
            case 10:
                updateUserProfile(currentUser, users, userCount);
                break;
            case 0:
                if (userCart.itemCount > 0) {
                    printf(COLOR_YELLOW "\nNote: Items in your active cart will be discarded on logout.\n" COLOR_RESET);
                    printf("Are you sure you want to log out? [1] Yes  [2] No: ");
                    if (getIntInput("", 1, 2) != 1) {
                        break;
                    }
                }
                printf(COLOR_GREEN "Successfully logged out.\n" COLOR_RESET);
                return;
        }
    }
}

int main(void) {
    enableVirtualTerminalProcessing();

    // Ensure database & seed data exists
    seedInitialData();

    // In-memory runtime data arrays
    Product products[MAX_PRODUCTS];
    User users[MAX_USERS];
    Order orders[MAX_ORDERS];

    int productCount = loadProducts(products, MAX_PRODUCTS);
    int userCount = loadUsers(users, MAX_USERS);
    int orderCount = loadOrders(orders, MAX_ORDERS);

    while (1) {
        clearScreen();
        printf(COLOR_BOLD COLOR_CYAN);
        printf("======================================================================\n");
        printf("               ONLINE SHOPPING CART & E-COMMERCE SYSTEM               \n");
        printf("                      Written in Standard C Language                  \n");
        printf("======================================================================\n");
        printf(COLOR_RESET);

        printf("\n  [1] Customer Login\n");
        printf("  [2] Register New Customer Account\n");
        printf("  [3] Administrator Login\n");
        printf("  [4] Browse Products as Guest\n");
        printf("  [0] Exit Application\n");
        printDivider('-', 70);

        int mainChoice = getIntInput("Choose an option: ", 0, 4);
        if (mainChoice == 0) {
            clearScreen();
            printf(COLOR_GREEN "\nThank you for using Online Shopping Cart System. Goodbye!\n\n" COLOR_RESET);
            break;
        }

        switch (mainChoice) {
            case 1: { // Customer Login
                clearScreen();
                printBanner("CUSTOMER LOGIN");
                char uname[MAX_USERNAME_LEN];
                char pass[MAX_PASSWORD_LEN];
                getStringInput("Username: ", uname, sizeof(uname));
                getStringInput("Password: ", pass, sizeof(pass));

                User *u = authenticateUser(users, userCount, uname, pass);
                if (u && u->role == ROLE_CUSTOMER) {
                    printf(COLOR_GREEN "\nLogin successful! Welcome back, %s.\n" COLOR_RESET, u->fullName);
                    pauseScreen();
                    customerSession(u, products, &productCount, users, userCount, orders, &orderCount);
                } else {
                    printf(COLOR_RED "\nInvalid username or password (or not a customer account).\n" COLOR_RESET);
                    pauseScreen();
                }
                break;
            }
            case 2: // Register Customer
                registerUser(users, &userCount, MAX_USERS);
                break;
            case 3: { // Admin Login
                clearScreen();
                printBanner("ADMINISTRATOR LOGIN");
                char uname[MAX_USERNAME_LEN];
                char pass[MAX_PASSWORD_LEN];
                getStringInput("Admin Username: ", uname, sizeof(uname));
                getStringInput("Admin Password: ", pass, sizeof(pass));

                User *admin = authenticateUser(users, userCount, uname, pass);
                if (admin && admin->role == ROLE_ADMIN) {
                    printf(COLOR_GREEN "\nAdmin authentication successful! Access granted.\n" COLOR_RESET);
                    pauseScreen();
                    adminDashboard(products, &productCount, MAX_PRODUCTS,
                                   users, &userCount, MAX_USERS,
                                   orders, &orderCount, MAX_ORDERS);
                } else {
                    printf(COLOR_RED "\nAccess Denied: Invalid administrator credentials!\n" COLOR_RESET);
                    pauseScreen();
                }
                break;
            }
            case 4: // Guest Browse
                browseCatalogMenu(products, productCount, NULL);
                break;
        }
    }

    return 0;
}
