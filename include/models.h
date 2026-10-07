#ifndef MODELS_H
#define MODELS_H

#define MAX_NAME_LEN 64
#define MAX_DESC_LEN 128
#define MAX_CAT_LEN 32
#define MAX_USERNAME_LEN 32
#define MAX_PASSWORD_LEN 32
#define MAX_ADDRESS_LEN 128
#define MAX_PHONE_LEN 20
#define MAX_EMAIL_LEN 64
#define MAX_CODE_LEN 16
#define MAX_CART_ITEMS 50
#define MAX_ORDER_ITEMS 50
#define MAX_PRODUCTS 100
#define MAX_USERS 100
#define MAX_ORDERS 200

typedef enum {
    ROLE_CUSTOMER = 1,
    ROLE_ADMIN = 2
} UserRole;

typedef enum {
    ORDER_PENDING = 0,
    ORDER_CONFIRMED,
    ORDER_SHIPPED,
    ORDER_DELIVERED,
    ORDER_CANCELLED
} OrderStatus;

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char category[MAX_CAT_LEN];
    char description[MAX_DESC_LEN];
    double price;
    int stock;
    double rating; // e.g. 4.5
} Product;

typedef struct {
    int productId;
    char productName[MAX_NAME_LEN];
    double unitPrice;
    int quantity;
} CartItem;

typedef struct {
    CartItem items[MAX_CART_ITEMS];
    int itemCount;
    char couponCode[MAX_CODE_LEN];
    double discountPercentage;
    double flatDiscount;
} Cart;

typedef struct {
    int id;
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    char fullName[MAX_NAME_LEN];
    char email[MAX_EMAIL_LEN];
    char phone[MAX_PHONE_LEN];
    char address[MAX_ADDRESS_LEN];
    UserRole role;
} User;

typedef struct {
    int orderId;
    int userId;
    char username[MAX_USERNAME_LEN];
    CartItem items[MAX_ORDER_ITEMS];
    int itemCount;
    double subtotal;
    double discount;
    double tax;
    double shipping;
    double totalAmount;
    char paymentMethod[32];
    char shippingAddress[MAX_ADDRESS_LEN];
    char orderDate[32];
    OrderStatus status;
} Order;

#endif // MODELS_H

