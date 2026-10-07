#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// ANSI Colors for Console UI
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_WHITE   "\033[1;37m"

void enableVirtualTerminalProcessing(void);
void clearScreen(void);
void pauseScreen(void);
void printBanner(const char *title);
void printDivider(char ch, int length);
void getStringInput(const char *prompt, char *buffer, int maxLen);
int getIntInput(const char *prompt, int minVal, int maxVal);
double getDoubleInput(const char *prompt, double minVal, double maxVal);
void getCurrentTimestamp(char *buffer, int maxLen);
void trimWhitespace(char *str);

#endif // COMMON_H
