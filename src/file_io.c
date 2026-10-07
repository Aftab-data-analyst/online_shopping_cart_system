#include "file_io.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(d) _mkdir(d)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(d) mkdir(d, 0755)
#endif

void ensureDataDirectoryExists(void) {
    MKDIR("data");
}

int loadProducts(Product products[], int maxCapacity) {
    FILE *fp = fopen(PRODUCTS_FILE, "r");
    if (!fp) {
        return 0;
    }

    int count = 0;
    char line[512];
    while (count < maxCapacity && fgets(line, sizeof(line), fp)) {
        trimWhitespace(line);
        if (strlen(line) == 0 || line[0] == '#') {
            continue;
        }

        Product p;
        char *token = strtok(line, "|");
        if (!token) continue;
        p.id = atoi(token);

        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(p.name, token, MAX_NAME_LEN - 1);
        p.name[MAX_NAME_LEN - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(p.category, token, MAX_CAT_LEN - 1);
        p.category[MAX_CAT_LEN - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(p.description, token, MAX_DESC_LEN - 1);
        p.description[MAX_DESC_LEN - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        p.price = atof(token);

        token = strtok(NULL, "|");
        if (!token) continue;
        p.stock = atoi(token);

        token = strtok(NULL, "|");
        if (!token) continue;
        p.rating = atof(token);

        products[count++] = p;
    }

    fclose(fp);
    return count;
}

bool saveProducts(const Product products[], int count) {
    ensureDataDirectoryExists();
    FILE *fp = fopen(PRODUCTS_FILE, "w");
    if (!fp) {
        return false;
    }

    fprintf(fp, "# id|name|category|description|price|stock|rating\n");
    for (int i = 0; i < count; ++i) {
        fprintf(fp, "%d|%s|%s|%s|%.2f|%d|%.1f\n",
                products[i].id,
                products[i].name,
                products[i].category,
                products[i].description,
                products[i].price,
                products[i].stock,
                products[i].rating);
    }

    fclose(fp);
    return true;
}

int loadUsers(User users[], int maxCapacity) {
    FILE *fp = fopen(USERS_FILE, "r");
    if (!fp) {
        return 0;
    }

    int count = 0;
    char line[512];
    while (count < maxCapacity && fgets(line, sizeof(line), fp)) {
        trimWhitespace(line);
        if (strlen(line) == 0 || line[0] == '#') {
            continue;
        }

        User u;
        char *token = strtok(line, "|");
        if (!token) continue;
        u.id = atoi(token);

        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(u.username, token, MAX_USERNAME_LEN - 1);
        u.username[MAX_USERNAME_LEN - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(u.password, token, MAX_PASSWORD_LEN - 1);
        u.password[MAX_PASSWORD_LEN - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(u.fullName, token, MAX_NAME_LEN - 1);
        u.fullName[MAX_NAME_LEN - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(u.email, token, MAX_EMAIL_LEN - 1);
        u.email[MAX_EMAIL_LEN - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(u.phone, token, MAX_PHONE_LEN - 1);
        u.phone[MAX_PHONE_LEN - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        strncpy(u.address, token, MAX_ADDRESS_LEN - 1);
        u.address[MAX_ADDRESS_LEN - 1] = '\0';

        token = strtok(NULL, "|");
        if (!token) continue;
        u.role = (UserRole)atoi(token);

        users[count++] = u;
    }

    fclose(fp);
    return count;
}

bool saveUsers(const User users[], int count) {
    ensureDataDirectoryExists();
    FILE *fp = fopen(USERS_FILE, "w");
    if (!fp) {
        return false;
    }

    fprintf(fp, "# id|username|password|fullName|email|phone|address|role\n");
    for (int i = 0; i < count; ++i) {
        fprintf(fp, "%d|%s|%s|%s|%s|%s|%s|%d\n",
                users[i].id,
                users[i].username,
                users[i].password,
                users[i].fullName,
                users[i].email,
                users[i].phone,
                users[i].address,
                users[i].role);
    }

    fclose(fp);
    return true;
}

int loadOrders(Order orders[], int maxCapacity) {
    FILE *fp = fopen(ORDERS_FILE, "r");
    if (!fp) {
        return 0;
    }

    int count = 0;
    char line[512];
    while (count < maxCapacity && fgets(line, sizeof(line), fp)) {
        trimWhitespace(line);
        if (strlen(line) == 0 || line[0] == '#') {
            continue;
        }

        if (strncmp(line, "ORDER|", 6) == 0) {
            Order o;
            memset(&o, 0, sizeof(Order));

            // Format: ORDER|orderId|userId|username|subtotal|discount|tax|shipping|totalAmount|paymentMethod|shippingAddress|orderDate|status|itemCount
            char *ptr = line + 6;
            char *token = strtok(ptr, "|");
            if (!token) continue;
            o.orderId = atoi(token);

            token = strtok(NULL, "|");
            if (!token) continue;
            o.userId = atoi(token);

            token = strtok(NULL, "|");
            if (!token) continue;
            strncpy(o.username, token, MAX_USERNAME_LEN - 1);

            token = strtok(NULL, "|");
            if (!token) continue;
            o.subtotal = atof(token);

            token = strtok(NULL, "|");
            if (!token) continue;
            o.discount = atof(token);

            token = strtok(NULL, "|");
            if (!token) continue;
            o.tax = atof(token);

            token = strtok(NULL, "|");
            if (!token) continue;
            o.shipping = atof(token);

            token = strtok(NULL, "|");
            if (!token) continue;
            o.totalAmount = atof(token);

            token = strtok(NULL, "|");
            if (!token) continue;
            strncpy(o.paymentMethod, token, sizeof(o.paymentMethod) - 1);

            token = strtok(NULL, "|");
            if (!token) continue;
            strncpy(o.shippingAddress, token, MAX_ADDRESS_LEN - 1);

            token = strtok(NULL, "|");
            if (!token) continue;
            strncpy(o.orderDate, token, sizeof(o.orderDate) - 1);

            token = strtok(NULL, "|");
            if (!token) continue;
            o.status = (OrderStatus)atoi(token);

            token = strtok(NULL, "|");
            if (!token) continue;
            int totalItems = atoi(token);
            o.itemCount = 0;

            // Read the subsequent item lines
            for (int j = 0; j < totalItems; ++j) {
                if (fgets(line, sizeof(line), fp)) {
                    trimWhitespace(line);
                    if (strncmp(line, "ITEM|", 5) == 0) {
                        CartItem ci;
                        char *iPtr = line + 5;
                        char *itok = strtok(iPtr, "|");
                        if (!itok) continue;
                        ci.productId = atoi(itok);

                        itok = strtok(NULL, "|");
                        if (!itok) continue;
                        strncpy(ci.productName, itok, MAX_NAME_LEN - 1);
                        ci.productName[MAX_NAME_LEN - 1] = '\0';

                        itok = strtok(NULL, "|");
                        if (!itok) continue;
                        ci.unitPrice = atof(itok);

                        itok = strtok(NULL, "|");
                        if (!itok) continue;
                        ci.quantity = atoi(itok);

                        if (o.itemCount < MAX_ORDER_ITEMS) {
                            o.items[o.itemCount++] = ci;
                        }
                    }
                }
            }
            orders[count++] = o;
        }
    }

    fclose(fp);
    return count;
}

bool saveOrders(const Order orders[], int count) {
    ensureDataDirectoryExists();
    FILE *fp = fopen(ORDERS_FILE, "w");
    if (!fp) {
        return false;
    }

    fprintf(fp, "# Orders Database\n");
    for (int i = 0; i < count; ++i) {
        const Order *o = &orders[i];
        fprintf(fp, "ORDER|%d|%d|%s|%.2f|%.2f|%.2f|%.2f|%.2f|%s|%s|%s|%d|%d\n",
                o->orderId,
                o->userId,
                o->username,
                o->subtotal,
                o->discount,
                o->tax,
                o->shipping,
                o->totalAmount,
                o->paymentMethod,
                o->shippingAddress,
                o->orderDate,
                o->status,
                o->itemCount);

        for (int j = 0; j < o->itemCount; ++j) {
            fprintf(fp, "ITEM|%d|%s|%.2f|%d\n",
                    o->items[j].productId,
                    o->items[j].productName,
                    o->items[j].unitPrice,
                    o->items[j].quantity);
        }
    }

    fclose(fp);
    return true;
}

bool appendOrder(const Order *order) {
    ensureDataDirectoryExists();
    FILE *fp = fopen(ORDERS_FILE, "a");
    if (!fp) {
        return false;
    }

    fprintf(fp, "ORDER|%d|%d|%s|%.2f|%.2f|%.2f|%.2f|%.2f|%s|%s|%s|%d|%d\n",
            order->orderId,
            order->userId,
            order->username,
            order->subtotal,
            order->discount,
            order->tax,
            order->shipping,
            order->totalAmount,
            order->paymentMethod,
            order->shippingAddress,
            order->orderDate,
            order->status,
            order->itemCount);

    for (int j = 0; j < order->itemCount; ++j) {
        fprintf(fp, "ITEM|%d|%s|%.2f|%d\n",
                order->items[j].productId,
                order->items[j].productName,
                order->items[j].unitPrice,
                order->items[j].quantity);
    }

    fclose(fp);
    return true;
}

void seedInitialData(void) {
    ensureDataDirectoryExists();

    // Check if products exist
    FILE *fpProd = fopen(PRODUCTS_FILE, "r");
    if (!fpProd) {
        Product initialProducts[] = {
            {101, "Apple iPhone 15 Pro", "Electronics", "128GB Titanium smartphone with A17 Pro chip", 999.00, 15, 4.8},
            {102, "Sony WH-1000XM5", "Electronics", "Wireless noise-canceling high-res headphones", 399.00, 25, 4.7},
            {103, "Dell XPS 15 Laptop", "Electronics", "Intel Core i9, 32GB RAM, 1TB SSD, OLED display", 1850.00, 8, 4.9},
            {104, "Logitech MX Master 3S", "Electronics", "Ergonomic wireless performance mouse", 99.00, 40, 4.6},
            {105, "Classic Cotton T-Shirt", "Clothing", "Premium 100% breathable organic cotton crewneck", 24.50, 80, 4.3},
            {106, "Levi's 511 Slim Jeans", "Clothing", "Comfort stretch denim classic slim fit jeans", 68.00, 50, 4.5},
            {107, "The Pragmatic Programmer", "Books", "20th Anniversary Edition by Hunt & Thomas", 45.00, 30, 4.9},
            {108, "Clean Code by Uncle Bob", "Books", "A handbook of agile software craftsmanship", 42.00, 22, 4.8},
            {109, "Organic Dark Roast Coffee", "Groceries", "1kg Whole Arabica beans, artisanal roasted", 18.50, 60, 4.6},
            {110, "Stainless Steel Water Bottle", "Home", "1 Litre insulated double-wall flask (24h cold)", 22.00, 45, 4.4}
        };
        int numProducts = sizeof(initialProducts) / sizeof(initialProducts[0]);
        saveProducts(initialProducts, numProducts);
    } else {
        fclose(fpProd);
    }

    // Check if users exist
    FILE *fpUser = fopen(USERS_FILE, "r");
    if (!fpUser) {
        User initialUsers[] = {
            {1, "admin", "admin123", "System Administrator", "admin@shopping.com", "+1-800-555-0199", "HQ Admin Suite 100", ROLE_ADMIN},
            {2, "john", "password123", "John Doe", "john.doe@example.com", "+1-555-234-5678", "742 Evergreen Terrace, Springfield", ROLE_CUSTOMER}
        };
        int numUsers = sizeof(initialUsers) / sizeof(initialUsers[0]);
        saveUsers(initialUsers, numUsers);
    } else {
        fclose(fpUser);
    }
}
