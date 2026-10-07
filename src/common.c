#include "common.h"
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

void enableVirtualTerminalProcessing(void) {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[H\033[J");
#endif
}

void pauseScreen(void) {
    printf(COLOR_YELLOW "\nPress Enter to continue..." COLOR_RESET);
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // discard remaining input if any
    }
}

void printDivider(char ch, int length) {
    for (int i = 0; i < length; ++i) {
        putchar(ch);
    }
    putchar('\n');
}

void printBanner(const char *title) {
    printf(COLOR_CYAN);
    printDivider('=', 70);
    printf("  %s\n", title);
    printDivider('=', 70);
    printf(COLOR_RESET);
}

void trimWhitespace(char *str) {
    if (!str) return;
    // Trim leading space
    char *start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
    // Trim trailing space
    int len = (int)strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}

void getStringInput(const char *prompt, char *buffer, int maxLen) {
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, maxLen, stdin) != NULL) {
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            }
            trimWhitespace(buffer);
            if (strlen(buffer) > 0) {
                break;
            }
        }
        printf(COLOR_RED "Input cannot be empty. Please try again.\n" COLOR_RESET);
    }
}

int getIntInput(const char *prompt, int minVal, int maxVal) {
    char buffer[64];
    int value;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            char extra;
            if (sscanf(buffer, "%d %c", &value, &extra) == 1) {
                if (value >= minVal && value <= maxVal) {
                    return value;
                }
            }
        }
        printf(COLOR_RED "Invalid input! Please enter a number between %d and %d.\n" COLOR_RESET, minVal, maxVal);
    }
}

double getDoubleInput(const char *prompt, double minVal, double maxVal) {
    char buffer[64];
    double value;
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            char extra;
            if (sscanf(buffer, "%lf %c", &value, &extra) == 1) {
                if (value >= minVal && value <= maxVal) {
                    return value;
                }
            }
        }
        printf(COLOR_RED "Invalid input! Please enter a value between %.2f and %.2f.\n" COLOR_RESET, minVal, maxVal);
    }
}

void getCurrentTimestamp(char *buffer, int maxLen) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    if (t != NULL) {
        strftime(buffer, maxLen, "%Y-%m-%d %H:%M:%S", t);
    } else {
        snprintf(buffer, maxLen, "Unknown Time");
    }
}

