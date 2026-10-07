#include "cart.h"
#include "catalog.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void initCart(Cart *cart) {
    if (!cart) return;
    cart->itemCount = 0;
    cart->couponCode[0] = '\0';
    cart->discountPercentage = 0.0;
    cart->flatDiscount = 0.0;
}

bool addToCart(Cart *cart, const Product products[], int productCount, int productId, int quantity) {
    if (!cart || quantity <= 0) {
        printf(COLOR_RED "Invalid quantity!\n" COLOR_RESET);
        return false;
    }

    // Find product in catalog
    const Product *found = NULL;
    for (int i = 0; i < productCount; ++i) {
        if (products[i].id == productId) {
            found = &products[i];
            break;
        }
    }

    if (!found) {
        printf(COLOR_RED "Error: Product ID %d not found in catalog!\n" COLOR_RESET, productId);
        return false;
    }

    // Check if already in cart
    int existingIndex = -1;
    for (int i = 0; i < cart->itemCount; ++i) {
        if (cart->items[i].productId == productId) {
            existingIndex = i;
            break;
        }
    }

    int currentInCart = (existingIndex != -1) ? cart->items[existingIndex].quantity : 0;
    if (currentInCart + quantity > found->stock) {
        printf(COLOR_RED "Error: Requested quantity exceeds available stock!\n" COLOR_RESET);
        printf(COLOR_YELLOW "Available in stock: %d | Currently in your cart: %d\n" COLOR_RESET,
               found->stock, currentInCart);
        return false;
    }

    if (existingIndex != -1) {
        cart->items[existingIndex].quantity += quantity;
        printf(COLOR_GREEN "Updated '%s' quantity to %d in your cart.\n" COLOR_RESET,
               found->name, cart->items[existingIndex].quantity);
    } else {
        if (cart->itemCount >= MAX_CART_ITEMS) {
            printf(COLOR_RED "Cart is full! Cannot add more distinct items.\n" COLOR_RESET);
            return false;
        }
        CartItem *item = &cart->items[cart->itemCount++];
        item->productId = found->id;
        strncpy(item->productName, found->name, MAX_NAME_LEN - 1);
        item->productName[MAX_NAME_LEN - 1] = '\0';
        item->unitPrice = found->price;
        item->quantity = quantity;

        printf(COLOR_GREEN "Added %d x '%s' ($%.2f each) to your cart!\n" COLOR_RESET,
               quantity, found->name, found->price);
    }

    return true;
}

bool removeFromCart(Cart *cart, int productId) {
    if (!cart || cart->itemCount == 0) {
        printf(COLOR_YELLOW "Your cart is already empty.\n" COLOR_RESET);
        return false;
    }

    int index = -1;
    for (int i = 0; i < cart->itemCount; ++i) {
        if (cart->items[i].productId == productId) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf(COLOR_RED "Product ID %d is not in your cart.\n" COLOR_RESET, productId);
        return false;
    }

    printf(COLOR_GREEN "Removed '%s' from your cart.\n" COLOR_RESET, cart->items[index].productName);

    for (int i = index; i < cart->itemCount - 1; ++i) {
        cart->items[i] = cart->items[i + 1];
    }
    cart->itemCount--;
    return true;
}

bool updateCartQuantity(Cart *cart, const Product products[], int productCount, int productId, int newQuantity) {
    if (!cart) return false;

    if (newQuantity == 0) {
        return removeFromCart(cart, productId);
    }

    if (newQuantity < 0) {
        printf(COLOR_RED "Quantity cannot be negative.\n" COLOR_RESET);
        return false;
    }

    int cartIndex = -1;
    for (int i = 0; i < cart->itemCount; ++i) {
        if (cart->items[i].productId == productId) {
            cartIndex = i;
            break;
        }
    }

    if (cartIndex == -1) {
        printf(COLOR_RED "Product ID %d not found in your cart.\n" COLOR_RESET, productId);
        return false;
    }

    // Check catalog stock
    const Product *found = NULL;
    for (int i = 0; i < productCount; ++i) {
        if (products[i].id == productId) {
            found = &products[i];
            break;
        }
    }

    if (found && newQuantity > found->stock) {
        printf(COLOR_RED "Cannot update quantity to %d. Only %d units available in stock.\n" COLOR_RESET,
               newQuantity, found->stock);
        return false;
    }

    cart->items[cartIndex].quantity = newQuantity;
    printf(COLOR_GREEN "Updated quantity of '%s' to %d.\n" COLOR_RESET,
           cart->items[cartIndex].productName, newQuantity);
    return true;
}

void clearCart(Cart *cart) {
    if (!cart) return;
    initCart(cart);
    printf(COLOR_GREEN "Shopping cart cleared successfully.\n" COLOR_RESET);
}

void calculateTotals(const Cart *cart, double *subtotal, double *discount, double *tax, double *shipping, double *grandTotal) {
    *subtotal = 0.0;
    for (int i = 0; i < cart->itemCount; ++i) {
        *subtotal += cart->items[i].unitPrice * cart->items[i].quantity;
    }

    *discount = 0.0;
    if (cart->discountPercentage > 0.0) {
        *discount += (*subtotal * cart->discountPercentage / 100.0);
    }
    if (cart->flatDiscount > 0.0) {
        *discount += cart->flatDiscount;
    }
    if (*discount > *subtotal) {
        *discount = *subtotal;
    }

    double discountedSubtotal = *subtotal - *discount;

    // Standard 8% sales tax on discounted subtotal
    *tax = discountedSubtotal * 0.08;

    // Free shipping if order >= $100 or special promo FREESHIP
    if (discountedSubtotal >= 100.0 || strcmp(cart->couponCode, "FREESHIP") == 0 || *subtotal == 0.0) {
        *shipping = 0.0;
    } else {
        *shipping = 10.0; // Flat $10 standard shipping
    }

    *grandTotal = discountedSubtotal + *tax + *shipping;
}

bool applyCouponCode(Cart *cart, const char *code) {
    if (!cart) return false;
    if (cart->itemCount == 0) {
        printf(COLOR_YELLOW "Cart is empty. Add products before applying coupons.\n" COLOR_RESET);
        return false;
    }

    char upperCode[32];
    int len = 0;
    while (code[len] != '\0' && len < 31) {
        upperCode[len] = (char)toupper((unsigned char)code[len]);
        len++;
    }
    upperCode[len] = '\0';

    if (strcmp(upperCode, "SAVE10") == 0) {
        strncpy(cart->couponCode, upperCode, MAX_CODE_LEN - 1);
        cart->discountPercentage = 10.0;
        cart->flatDiscount = 0.0;
        printf(COLOR_GREEN "Coupon applied! 10%% discount on your order.\n" COLOR_RESET);
        return true;
    } else if (strcmp(upperCode, "SAVE20") == 0) {
        strncpy(cart->couponCode, upperCode, MAX_CODE_LEN - 1);
        cart->discountPercentage = 20.0;
        cart->flatDiscount = 0.0;
        printf(COLOR_GREEN "Coupon applied! 20%% discount on your order.\n" COLOR_RESET);
        return true;
    } else if (strcmp(upperCode, "BIGSALE") == 0) {
        strncpy(cart->couponCode, upperCode, MAX_CODE_LEN - 1);
        cart->discountPercentage = 25.0;
        cart->flatDiscount = 0.0;
        printf(COLOR_GREEN "Super deal applied! 25%% discount on your order.\n" COLOR_RESET);
        return true;
    } else if (strcmp(upperCode, "FLAT50") == 0) {
        double subtotal = 0.0;
        for (int i = 0; i < cart->itemCount; ++i) {
            subtotal += cart->items[i].unitPrice * cart->items[i].quantity;
        }
        if (subtotal < 150.0) {
            printf(COLOR_RED "Coupon 'FLAT50' requires a minimum order subtotal of $150.00!\n" COLOR_RESET);
            return false;
        }
        strncpy(cart->couponCode, upperCode, MAX_CODE_LEN - 1);
        cart->discountPercentage = 0.0;
        cart->flatDiscount = 50.0;
        printf(COLOR_GREEN "Coupon applied! $50 flat discount applied.\n" COLOR_RESET);
        return true;
    } else if (strcmp(upperCode, "FREESHIP") == 0) {
        strncpy(cart->couponCode, upperCode, MAX_CODE_LEN - 1);
        cart->discountPercentage = 0.0;
        cart->flatDiscount = 0.0;
        printf(COLOR_GREEN "Coupon applied! Free shipping unlocked.\n" COLOR_RESET);
        return true;
    } else {
        printf(COLOR_RED "Invalid promo code '%s'. Valid codes: SAVE10, SAVE20, BIGSALE, FLAT50, FREESHIP\n" COLOR_RESET, code);
        return false;
    }
}

void displayCart(const Cart *cart) {
    if (!cart || cart->itemCount == 0) {
        printf(COLOR_YELLOW "\nYour shopping cart is currently empty!\n" COLOR_RESET);
        return;
    }

    printf(COLOR_CYAN);
    printDivider('=', 78);
    printf("| %-4s | %-28s | %-10s | %-8s | %-12s |\n",
           "ID", "Product Name", "Unit Price", "Quantity", "Total Price");
    printDivider('=', 78);
    printf(COLOR_RESET);

    for (int i = 0; i < cart->itemCount; ++i) {
        double rowTotal = cart->items[i].unitPrice * cart->items[i].quantity;
        printf("| %-4d | %-28.28s | $%9.2f | %8d | $%11.2f |\n",
               cart->items[i].productId,
               cart->items[i].productName,
               cart->items[i].unitPrice,
               cart->items[i].quantity,
               rowTotal);
    }

    printDivider('-', 78);

    double subtotal, discount, tax, shipping, grandTotal;
    calculateTotals(cart, &subtotal, &discount, &tax, &shipping, &grandTotal);

    printf("  %-45s : $%10.2f\n", "Subtotal", subtotal);
    if (strlen(cart->couponCode) > 0) {
        printf(COLOR_GREEN "  Discount Applied [%-10s]              : -$%9.2f\n" COLOR_RESET,
               cart->couponCode, discount);
    }
    printf("  %-45s : $%10.2f\n", "Estimated Tax (8%)", tax);
    if (shipping == 0.0) {
        printf(COLOR_GREEN "  Shipping Fee                                  : FREE\n" COLOR_RESET);
    } else {
        printf("  %-45s : $%10.2f\n", "Shipping Fee (Orders < $100)", shipping);
    }
    printDivider('=', 78);
    printf(COLOR_BOLD COLOR_GREEN "  FINAL ORDER TOTAL                             : $%10.2f\n" COLOR_RESET, grandTotal);
    printDivider('=', 78);
}
