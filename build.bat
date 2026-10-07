@echo off
echo ========================================================
echo Building Online Shopping Cart System (C Language)
echo ========================================================

gcc -Wall -Wextra -std=c99 -Iinclude src\common.c src\file_io.c src\catalog.c src\cart.c src\auth.c src\order.c src\admin.c src\main.c -o shopping_cart.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [SUCCESS] Build succeeded! Run with: shopping_cart.exe
) else (
    echo.
    echo [ERROR] Build failed! Check compiler errors above.
)

