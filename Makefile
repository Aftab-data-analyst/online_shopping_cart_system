CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -O2
SRC = src/common.c src/file_io.c src/catalog.c src/cart.c src/auth.c src/order.c src/admin.c src/main.c
TARGET = shopping_cart

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe
