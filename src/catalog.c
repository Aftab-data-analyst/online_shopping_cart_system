#include "catalog.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

static void toLowerCase(const char *src, char *dest, int maxLen) {
    int i = 0;
    while (src[i] != '\0' && i < maxLen - 1) {
        dest[i] = (char)tolower((unsigned char)src[i]);
        i++;
    }
    dest[i] = '\0';
}

void displayProductRow(const Product *p) {
    const char *stockColor = (p->stock == 0) ? COLOR_RED : ((p->stock < 10) ? COLOR_YELLOW : COLOR_GREEN);
    printf("| %-4d | %-24.24s | %-12.12s | %8.2f | %s%5d%s | %5.1f | %-28.28s |\n",
           p->id,
           p->name,
           p->category,
           p->price,
           stockColor,
           p->stock,
           COLOR_RESET,
           p->rating,
           p->description);
}

static void printCatalogHeader(void) {
    printf(COLOR_CYAN);
    printDivider('-', 97);
    printf("| %-4s | %-24s | %-12s | %-8s | %-5s | %-5s | %-28s |\n",
           "ID", "Product Name", "Category", "Price($)", "Stock", "Rate*", "Description");
    printDivider('-', 97);
    printf(COLOR_RESET);
}

void displayCatalog(const Product products[], int count) {
    if (count <= 0) {
        printf(COLOR_YELLOW "No products available in the catalog.\n" COLOR_RESET);
        return;
    }

    printCatalogHeader();
    for (int i = 0; i < count; ++i) {
        displayProductRow(&products[i]);
    }
    printDivider('-', 97);
    printf(COLOR_GREEN "Total Products Listed: %d\n" COLOR_RESET, count);
}

void displayCatalogPaginated(const Product products[], int count) {
    if (count <= 0) {
        printf(COLOR_YELLOW "No products available.\n" COLOR_RESET);
        return;
    }

    const int PAGE_SIZE = 8;
    int totalPages = (count + PAGE_SIZE - 1) / PAGE_SIZE;
    int currentPage = 1;

    while (1) {
        clearScreen();
        char bannerTitle[64];
        snprintf(bannerTitle, sizeof(bannerTitle), "PRODUCT CATALOG - Page %d of %d", currentPage, totalPages);
        printBanner(bannerTitle);

        printCatalogHeader();
        int startIndex = (currentPage - 1) * PAGE_SIZE;
        int endIndex = startIndex + PAGE_SIZE;
        if (endIndex > count) endIndex = count;

        for (int i = startIndex; i < endIndex; ++i) {
            displayProductRow(&products[i]);
        }
        printDivider('-', 97);

        printf("\nNavigation: [1] Next Page  [2] Previous Page  [0] Return to Menu\n");
        int choice = getIntInput("Choose an option: ", 0, 2);
        if (choice == 1) {
            if (currentPage < totalPages) currentPage++;
            else printf(COLOR_YELLOW "You are already on the last page!\n" COLOR_RESET);
        } else if (choice == 2) {
            if (currentPage > 1) currentPage--;
            else printf(COLOR_YELLOW "You are already on the first page!\n" COLOR_RESET);
        } else {
            break;
        }
    }
}

void searchProductsByName(const Product products[], int count, const char *keyword) {
    char lowerKeyword[64];
    toLowerCase(keyword, lowerKeyword, sizeof(lowerKeyword));

    int matches = 0;
    printCatalogHeader();
    for (int i = 0; i < count; ++i) {
        char lowerName[MAX_NAME_LEN];
        char lowerDesc[MAX_DESC_LEN];
        toLowerCase(products[i].name, lowerName, sizeof(lowerName));
        toLowerCase(products[i].description, lowerDesc, sizeof(lowerDesc));

        if (strstr(lowerName, lowerKeyword) != NULL || strstr(lowerDesc, lowerKeyword) != NULL) {
            displayProductRow(&products[i]);
            matches++;
        }
    }
    printDivider('-', 97);

    if (matches == 0) {
        printf(COLOR_YELLOW "No products found matching \"%s\".\n" COLOR_RESET, keyword);
    } else {
        printf(COLOR_GREEN "Found %d matching product(s).\n" COLOR_RESET, matches);
    }
}

void filterProductsByCategory(const Product products[], int count, const char *category) {
    char lowerCat[64];
    toLowerCase(category, lowerCat, sizeof(lowerCat));

    int matches = 0;
    printCatalogHeader();
    for (int i = 0; i < count; ++i) {
        char prodCat[MAX_CAT_LEN];
        toLowerCase(products[i].category, prodCat, sizeof(prodCat));

        if (strcmp(prodCat, lowerCat) == 0) {
            displayProductRow(&products[i]);
            matches++;
        }
    }
    printDivider('-', 97);

    if (matches == 0) {
        printf(COLOR_YELLOW "No products found in category \"%s\".\n" COLOR_RESET, category);
    } else {
        printf(COLOR_GREEN "Found %d product(s) in \"%s\".\n" COLOR_RESET, matches, category);
    }
}

void sortProductsByPrice(Product products[], int count, bool ascending) {
    for (int i = 0; i < count - 1; ++i) {
        for (int j = 0; j < count - i - 1; ++j) {
            bool condition = ascending ? (products[j].price > products[j + 1].price)
                                       : (products[j].price < products[j + 1].price);
            if (condition) {
                Product temp = products[j];
                products[j] = products[j + 1];
                products[j + 1] = temp;
            }
        }
    }
}

void sortProductsByRating(Product products[], int count) {
    for (int i = 0; i < count - 1; ++i) {
        for (int j = 0; j < count - i - 1; ++j) {
            if (products[j].rating < products[j + 1].rating) {
                Product temp = products[j];
                products[j] = products[j + 1];
                products[j + 1] = temp;
            }
        }
    }
}

Product* findProductById(Product products[], int count, int productId) {
    for (int i = 0; i < count; ++i) {
        if (products[i].id == productId) {
            return &products[i];
        }
    }
    return NULL;
}

void listCategories(const Product products[], int count) {
    char categories[MAX_PRODUCTS][MAX_CAT_LEN];
    int catCount = 0;

    for (int i = 0; i < count; ++i) {
        bool found = false;
        for (int j = 0; j < catCount; ++j) {
            if (strcmp(categories[j], products[i].category) == 0) {
                found = true;
                break;
            }
        }
        if (!found && catCount < MAX_PRODUCTS) {
            strncpy(categories[catCount], products[i].category, MAX_CAT_LEN - 1);
            categories[catCount][MAX_CAT_LEN - 1] = '\0';
            catCount++;
        }
    }

    printf(COLOR_CYAN "Available Categories:\n" COLOR_RESET);
    for (int i = 0; i < catCount; ++i) {
        printf("  %d. %s\n", i + 1, categories[i]);
    }
}
