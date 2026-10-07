#ifndef CATALOG_H
#define CATALOG_H

#include "models.h"
#include <stdbool.h>

void displayProductRow(const Product *p);
void displayCatalog(const Product products[], int count);
void displayCatalogPaginated(const Product products[], int count);
void searchProductsByName(const Product products[], int count, const char *keyword);
void filterProductsByCategory(const Product products[], int count, const char *category);
void sortProductsByPrice(Product products[], int count, bool ascending);
void sortProductsByRating(Product products[], int count);
Product* findProductById(Product products[], int count, int productId);
void listCategories(const Product products[], int count);

#endif // CATALOG_H

