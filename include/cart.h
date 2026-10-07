#ifndef CART_H
#define CART_H

#include "models.h"
#include <stdbool.h>

void initCart(Cart *cart);
bool addToCart(Cart *cart, const Product products[], int productCount, int productId, int quantity);
bool removeFromCart(Cart *cart, int productId);
bool updateCartQuantity(Cart *cart, const Product products[], int productCount, int productId, int newQuantity);
void clearCart(Cart *cart);
void displayCart(const Cart *cart);
bool applyCouponCode(Cart *cart, const char *code);
void calculateTotals(const Cart *cart, double *subtotal, double *discount, double *tax, double *shipping, double *grandTotal);

#endif // CART_H

