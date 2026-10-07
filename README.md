# 🛒 Online Shopping Cart System in C

A fully-featured, modular, and terminal-based **Online Shopping Cart & E-Commerce Management System** written in standard **C (C99)**. It provides a complete e-commerce lifecycle including catalog browsing, cart operations, promo codes, simulated checkout and payment, inventory tracking, invoice generation, customer accounts, and an administrator dashboard with data persistence.

---

## 🌟 Key Features

### 👤 Customer Features
- **User Authentication & Registration**: Secure customer registration and login with profile management (update name, address, contact details, password).
- **Product Discovery**:
  - Paginated product catalog view with stock indicators and ratings.
  - Search products by keyword or title (case-insensitive).
  - Filter products by category (Electronics, Clothing, Books, Groceries, Home, etc.).
  - Multi-criteria sorting: Price (Low to High / High to Low) and Rating.
- **Cart Management**:
  - Add items to cart with real-time inventory validation.
  - View cart with formatted itemized breakdown (Unit price, quantity, row total).
  - Modify quantities or remove items dynamically.
  - Empty / clear cart at any time.
- **Promotions & Discount Engine**:
  - `SAVE10`: 10% discount on order subtotal.
  - `SAVE20`: 20% discount on order subtotal.
  - `BIGSALE`: 25% discount on order subtotal.
  - `FLAT50`: Flat $50 discount on orders over $150.
  - `FREESHIP`: 100% discount on shipping costs.
- **Checkout & Multi-Mode Payment**:
  - Automated tax calculation (8%) and shipping computation (Free for orders over $100).
  - Multiple simulated payment options:
    - **Credit / Debit Card** (Card number, expiry date, CVV validation)
    - **UPI / Net Banking** (UPI ID validation)
    - **Cash on Delivery (COD)**
  - Automated inventory stock deduction upon confirmed order.
  - Generation of itemized printable purchase invoices with unique Order IDs.
- **Order History**: Review past purchases and inspect historical invoices.

---

### 🛡️ Administrator Features
- **Secure Admin Portal**: Dedicated administrator control panel (Default login: `admin` / `admin123`).
- **Product Management (CRUD)**:
  - Add new products (ID, name, category, description, price, stock, rating).
  - Update product attributes (price, stock, details).
  - Delete discontinued products.
- **Inventory & Stock Monitoring**: Low-stock alert report identifying any product with inventory $\le$ threshold.
- **Order Management**:
  - View all customer orders across the platform.
  - Transition order statuses (`PENDING`, `CONFIRMED`, `SHIPPED`, `DELIVERED`, `CANCELLED`).
  - View any customer's complete invoice breakdown.
- **Sales & Analytics**:
  - Total revenue generated, total units sold, average order value (AOV).
  - Total discounts granted, total tax collected, and order status distribution.

---

### 💾 Data Persistence
All data is stored in human-readable and structured text files under `data/`:
- `data/products.txt`: Product catalog inventory and specs.
- `data/users.txt`: User registry (credentials, roles, profiles).
- `data/orders.txt`: Complete order transaction records and line items.
- *Automatic Seeding*: If files are missing, the system seeds 10 sample products and initial accounts automatically.

---

## 📂 Project Architecture

```
online_shopping_cart_system/
│
├── include/                 # Header declarations
│   ├── admin.h              # Admin panel and inventory operations
│   ├── auth.h               # Authentication and user profiles
│   ├── cart.h               # Cart operations and coupon logic
│   ├── catalog.h            # Product catalog, search, filter, sort
│   ├── common.h             # Terminal colors, input validators, utilities
│   ├── file_io.h            # File persistence and initialization
│   ├── models.h             # Core data structures (Product, Cart, Order, User)
│   └── order.h              # Checkout flow, invoices, order tracking
│
├── src/                     # C implementation source files
│   ├── admin.c              # Admin dashboard routines
│   ├── auth.c               # Login and registration logic
│   ├── cart.c               # Shopping cart implementations
│   ├── catalog.c            # Catalog display and search algorithms
│   ├── common.c             # Cross-platform console helpers
│   ├── file_io.c            # Flat-file database read/write routines
│   ├── main.c               # Main CLI interface loop
│   └── order.c              # Checkout, payment simulation, and receipt printing
│
├── data/                    # Persistent storage (auto-created on first run)
│   ├── products.txt         # Seeded product inventory
│   ├── users.txt            # User accounts
│   └── orders.txt           # Orders history
│
├── build.bat                # Windows quick build script
├── Makefile                 # Make build configuration (Linux / macOS / MinGW)
├── .gitignore               # Ignored build artifacts and binaries
└── README.md                # System documentation
```

---

## 🚀 Building & Running

### Prerequisites
- Any standard C compiler supporting **C99** or later:
  - GCC / MinGW (`gcc`)
  - Clang (`clang`)
  - MSVC (`cl`)

---

### Option 1: Windows (Command Prompt / PowerShell)

Using GCC / MinGW:
```powershell
gcc -Wall -Wextra -std=c99 -Iinclude src/common.c src/file_io.c src/catalog.c src/cart.c src/auth.c src/order.c src/admin.c src/main.c -o shopping_cart.exe
.\shopping_cart.exe
```

Or run the batch script:
```cmd
build.bat
shopping_cart.exe
```

---

### Option 2: Linux / macOS

Using `make`:
```bash
make
./shopping_cart
```

Or directly using `gcc`:
```bash
gcc -Wall -Wextra -std=c99 -Iinclude src/*.c -o shopping_cart
./shopping_cart
```

---

## 🔑 Default Accounts

| Role | Username | Password | Notes |
|---|---|---|---|
| **Administrator** | `admin` | `admin123` | Full access to inventory, sales analytics, and order fulfillment |
| **Customer (Demo)** | `john` | `password123` | Pre-configured customer with default delivery address |

*Note: You can also register any number of new customer accounts directly from the main menu.*

---

## 🧾 Sample Checkout Invoice Output

```
==========================================================================
                  OFFICIAL PURCHASE INVOICE & RECEIPT             
==========================================================================
  Order ID       : #01001
  Order Date     : 2026-10-07 10:20:00
  Customer Name  : John Doe (User ID: 2)
  Delivery To    : 742 Evergreen Terrace, Springfield
  Payment Mode   : Card (**** 1234)
  Order Status   : CONFIRMED
--------------------------------------------------------------------------
  No.   Item Description              Unit Price  Qty     Total     
--------------------------------------------------------------------------
  1     Apple iPhone 15 Pro           $   999.00  1       $   999.00
  2     Sony WH-1000XM5               $   399.00  1       $   399.00
--------------------------------------------------------------------------
  Subtotal                                           : $   1398.00
  Discount                                           : -$   139.80
  Taxes (8%)                                         : $    100.66
  Shipping Charge                                    : FREE
==========================================================================
  GRAND TOTAL PAID                                   : $   1358.86
==========================================================================
       Thank you for shopping with us! Have a wonderful day!      
==========================================================================
```

---

## 📜 License
This project is open-source and released under the MIT License.

