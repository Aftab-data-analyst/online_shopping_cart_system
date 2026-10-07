#include "order.h"
#include "cart.h"
#include "file_io.h"
#include "common.h"
#include <stdio.h>
#include <string.h>

const char* getOrderStatusString(OrderStatus status) {
    switch (status) {
        case ORDER_PENDING:   return "PENDING";
        case ORDER_CONFIRMED: return "CONFIRMED";
        case ORDER_SHIPPED:   return "SHIPPED";
        case ORDER_DELIVERED: return "DELIVERED";
        case ORDER_CANCELLED: return "CANCELLED";
        default:              return "UNKNOWN";
    }
}

Order* findOrderById(Order orders[], int orderCount, int orderId) {
    for (int i = 0; i < orderCount; ++i) {
        if (orders[i].orderId == orderId) {
            return &orders[i];
        }
    }
    return NULL;
}

void printInvoice(const Order *order) {
    if (!order) return;

    printf(COLOR_CYAN);
    printDivider('=', 74);
    printf("                  OFFICIAL PURCHASE INVOICE & RECEIPT             \n");
    printDivider('=', 74);
    printf(COLOR_RESET);

    printf("  Order ID       : #%05d\n", order->orderId);
    printf("  Order Date     : %s\n", order->orderDate);
    printf("  Customer Name  : %s (User ID: %d)\n", order->username, order->userId);
    printf("  Delivery To    : %s\n", order->shippingAddress);
    printf("  Payment Mode   : %s\n", order->paymentMethod);
    printf("  Order Status   : " COLOR_GREEN "%s" COLOR_RESET "\n", getOrderStatusString(order->status));
    printDivider('-', 74);

    printf("  %-4s  %-28s  %-10s  %-6s  %-10s\n", "No.", "Item Description", "Unit Price", "Qty", "Total");
    printDivider('-', 74);
    for (int i = 0; i < order->itemCount; ++i) {
        double lineTotal = order->items[i].unitPrice * order->items[i].quantity;
        printf("  %-4d  %-28.28s  $%9.2f  %-6d  $%9.2f\n",
               i + 1,
               order->items[i].productName,
               order->items[i].unitPrice,
               order->items[i].quantity,
               lineTotal);
    }
    printDivider('-', 74);

    printf("  %-50s : $%10.2f\n", "Subtotal", order->subtotal);
    if (order->discount > 0.0) {
        printf(COLOR_GREEN "  %-50s : -$%9.2f\n" COLOR_RESET, "Discount", order->discount);
    }
    printf("  %-50s : $%10.2f\n", "Taxes (8%)", order->tax);
    if (order->shipping == 0.0) {
        printf(COLOR_GREEN "  %-50s : FREE\n" COLOR_RESET, "Shipping Charge");
    } else {
        printf("  %-50s : $%10.2f\n", "Shipping Charge", order->shipping);
    }
    printDivider('=', 74);
    printf(COLOR_BOLD COLOR_GREEN "  %-50s : $%10.2f\n" COLOR_RESET, "GRAND TOTAL PAID", order->totalAmount);
    printDivider('=', 74);
    printf("       Thank you for shopping with us! Have a wonderful day!      \n");
    printDivider('=', 74);
}

bool processCheckout(Cart *cart, User *user, Product products[], int productCount, Order orders[], int *orderCount, int maxOrders) {
    if (!cart || cart->itemCount == 0) {
        printf(COLOR_YELLOW "Your cart is empty! Add items before checkout.\n" COLOR_RESET);
        return false;
    }

    if (*orderCount >= maxOrders) {
        printf(COLOR_RED "Order database is at maximum limit.\n" COLOR_RESET);
        return false;
    }

    clearScreen();
    printBanner("SECURE ORDER CHECKOUT");

    // 1. Verify current stock for all items
    for (int i = 0; i < cart->itemCount; ++i) {
        int pid = cart->items[i].productId;
        int reqQty = cart->items[i].quantity;
        Product *prod = NULL;
        for (int p = 0; p < productCount; ++p) {
            if (products[p].id == pid) {
                prod = &products[p];
                break;
            }
        }
        if (!prod || prod->stock < reqQty) {
            printf(COLOR_RED "Checkout Error: '%s' has insufficient stock (Requested: %d, Available: %d)!\n" COLOR_RESET,
                   cart->items[i].productName, reqQty, (prod ? prod->stock : 0));
            printf(COLOR_YELLOW "Please adjust your cart quantity.\n" COLOR_RESET);
            pauseScreen();
            return false;
        }
    }

    // Display order summary
    displayCart(cart);

    // 2. Shipping Address
    char deliveryAddress[MAX_ADDRESS_LEN];
    printf("\nDelivery Address:\n");
    printf("  Default: %s\n", user->address);
    printf("Use default address? [1] Yes  [2] Enter different address: ");
    int addrChoice = getIntInput("", 1, 2);
    if (addrChoice == 1) {
        strncpy(deliveryAddress, user->address, MAX_ADDRESS_LEN - 1);
        deliveryAddress[MAX_ADDRESS_LEN - 1] = '\0';
    } else {
        getStringInput("Enter new delivery address: ", deliveryAddress, sizeof(deliveryAddress));
    }

    // 3. Payment Method
    printf("\nSelect Payment Method:\n");
    printf("  [1] Credit / Debit Card\n");
    printf("  [2] UPI / Online Banking\n");
    printf("  [3] Cash on Delivery (COD)\n");
    printf("  [0] Cancel Checkout\n");
    int payChoice = getIntInput("Choose payment method: ", 0, 3);
    if (payChoice == 0) {
        printf(COLOR_YELLOW "Checkout cancelled.\n" COLOR_RESET);
        pauseScreen();
        return false;
    }

    char paymentMethod[32];
    if (payChoice == 1) {
        char cardNumber[24], expDate[10], cvv[6];
        printf(COLOR_CYAN "\n--- Card Payment Simulation ---\n" COLOR_RESET);
        getStringInput("Enter 16-digit Card Number: ", cardNumber, sizeof(cardNumber));
        getStringInput("Enter Expiry Date (MM/YY): ", expDate, sizeof(expDate));
        getStringInput("Enter CVV (3 digits): ", cvv, sizeof(cvv));
        printf(COLOR_GREEN "Card validated successfully.\n" COLOR_RESET);
        snprintf(paymentMethod, sizeof(paymentMethod), "Card (**** %s)",
                 (strlen(cardNumber) >= 4) ? (cardNumber + strlen(cardNumber) - 4) : "XXXX");
    } else if (payChoice == 2) {
        char upiId[48];
        printf(COLOR_CYAN "\n--- UPI Payment Simulation ---\n" COLOR_RESET);
        getStringInput("Enter UPI ID (e.g. user@okaxis / user@upi): ", upiId, sizeof(upiId));
        printf(COLOR_GREEN "Payment authorization request approved.\n" COLOR_RESET);
        snprintf(paymentMethod, sizeof(paymentMethod), "UPI (%s)", upiId);
    } else {
        snprintf(paymentMethod, sizeof(paymentMethod), "Cash on Delivery");
    }

    // 4. Calculate final values
    double subtotal, discount, tax, shipping, grandTotal;
    calculateTotals(cart, &subtotal, &discount, &tax, &shipping, &grandTotal);

    // 5. Generate Order ID
    int nextOrderId = 1001;
    for (int i = 0; i < *orderCount; ++i) {
        if (orders[i].orderId >= nextOrderId) {
            nextOrderId = orders[i].orderId + 1;
        }
    }

    Order newOrder;
    memset(&newOrder, 0, sizeof(Order));
    newOrder.orderId = nextOrderId;
    newOrder.userId = user->id;
    strncpy(newOrder.username, user->username, MAX_USERNAME_LEN - 1);
    newOrder.itemCount = cart->itemCount;
    for (int i = 0; i < cart->itemCount; ++i) {
        newOrder.items[i] = cart->items[i];
    }
    newOrder.subtotal = subtotal;
    newOrder.discount = discount;
    newOrder.tax = tax;
    newOrder.shipping = shipping;
    newOrder.totalAmount = grandTotal;
    strncpy(newOrder.paymentMethod, paymentMethod, sizeof(newOrder.paymentMethod) - 1);
    strncpy(newOrder.shippingAddress, deliveryAddress, MAX_ADDRESS_LEN - 1);
    getCurrentTimestamp(newOrder.orderDate, sizeof(newOrder.orderDate));
    newOrder.status = ORDER_CONFIRMED;

    // 6. Deduct inventory
    for (int i = 0; i < cart->itemCount; ++i) {
        int pid = cart->items[i].productId;
        int qty = cart->items[i].quantity;
        for (int p = 0; p < productCount; ++p) {
            if (products[p].id == pid) {
                products[p].stock -= qty;
                break;
            }
        }
    }
    saveProducts(products, productCount);

    // 7. Save order
    orders[*orderCount] = newOrder;
    (*orderCount)++;
    saveOrders(orders, *orderCount);

    // 8. Clear cart
    initCart(cart);

    clearScreen();
    printf(COLOR_GREEN "\n>>> ORDER PLACED SUCCESSFULLY! <<<\n\n" COLOR_RESET);
    printInvoice(&newOrder);
    pauseScreen();
    return true;
}

void viewUserOrderHistory(const Order orders[], int orderCount, int userId) {
    clearScreen();
    printBanner("YOUR ORDER HISTORY");

    int userOrderCount = 0;
    for (int i = 0; i < orderCount; ++i) {
        if (orders[i].userId == userId) {
            userOrderCount++;
            printf("\nOrder #%d | Date: %s | Total: $%.2f | Status: %s\n",
                   orders[i].orderId, orders[i].orderDate, orders[i].totalAmount,
                   getOrderStatusString(orders[i].status));
            printf("Items: ");
            for (int j = 0; j < orders[i].itemCount; ++j) {
                printf("%s (x%d)%s", orders[i].items[j].productName, orders[i].items[j].quantity,
                       (j < orders[i].itemCount - 1) ? ", " : "\n");
            }
            printDivider('-', 60);
        }
    }

    if (userOrderCount == 0) {
        printf(COLOR_YELLOW "You have not placed any orders yet.\n" COLOR_RESET);
        pauseScreen();
        return;
    }

    printf("\nWould you like to view a detailed invoice for an order?\n");
    printf("Enter Order ID to view details (or 0 to return): ");
    int targetId = getIntInput("", 0, 999999);
    if (targetId != 0) {
        bool found = false;
        for (int i = 0; i < orderCount; ++i) {
            if (orders[i].userId == userId && orders[i].orderId == targetId) {
                clearScreen();
                printInvoice(&orders[i]);
                found = true;
                break;
            }
        }
        if (!found) {
            printf(COLOR_RED "Order ID #%d not found in your orders.\n" COLOR_RESET, targetId);
        }
        pauseScreen();
    }
}

void viewAllOrders(const Order orders[], int orderCount) {
    clearScreen();
    printBanner("ALL SYSTEM ORDERS (ADMIN VIEW)");

    if (orderCount == 0) {
        printf(COLOR_YELLOW "No orders recorded in the system yet.\n" COLOR_RESET);
        pauseScreen();
        return;
    }

    printf(COLOR_CYAN);
    printDivider('-', 95);
    printf("| %-7s | %-12s | %-19s | %-10s | %-12s | %-16s |\n",
           "Order#", "User", "Date", "Total($)", "Status", "Payment");
    printDivider('-', 95);
    printf(COLOR_RESET);

    for (int i = 0; i < orderCount; ++i) {
        printf("| #%-6d | %-12.12s | %-19.19s | $%9.2f | %-12.12s | %-16.16s |\n",
               orders[i].orderId,
               orders[i].username,
               orders[i].orderDate,
               orders[i].totalAmount,
               getOrderStatusString(orders[i].status),
               orders[i].paymentMethod);
    }
    printDivider('-', 95);

    printf("\nOptions: [1] View Order Invoice  [2] Return\n");
    int opt = getIntInput("Select option: ", 1, 2);
    if (opt == 1) {
        int oid = getIntInput("Enter Order ID: ", 1, 999999);
        for (int i = 0; i < orderCount; ++i) {
            if (orders[i].orderId == oid) {
                clearScreen();
                printInvoice(&orders[i]);
                pauseScreen();
                return;
            }
        }
        printf(COLOR_RED "Order ID #%d not found.\n" COLOR_RESET, oid);
        pauseScreen();
    }
}

bool updateOrderStatus(Order orders[], int orderCount, int orderId, OrderStatus newStatus) {
    Order *ord = findOrderById(orders, orderCount, orderId);
    if (!ord) {
        printf(COLOR_RED "Order #%d not found.\n" COLOR_RESET, orderId);
        return false;
    }

    ord->status = newStatus;
    saveOrders(orders, orderCount);
    printf(COLOR_GREEN "Order #%d status updated to %s.\n" COLOR_RESET, orderId, getOrderStatusString(newStatus));
    return true;
}
