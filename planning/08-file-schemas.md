# 08 — File Schemas (text + binary)

> Phase 1 planning document. This file defines the exact on-disk layout of every
> persistence file the app uses. **Repositories are the only code allowed to touch
> these files** (see `03-class-design.md` §L).
>
> Conventions:
> - Text files use **UTF-8**, **LF line endings**, and `|` as the delimiter unless
>   otherwise stated. A leading `#` makes a comment line.
> - Binary files store fixed-width, **trivially copyable POD record structs**
>   written with `ofstream(..., std::ios::binary)` and `.write(reinterpret_cast<const char*>(&r), sizeof(r))`.
> - Endianness is whatever the host writes (little-endian on Windows MinGW); files
>   are **not** portable across architectures — fine for a single-machine POS.
> - Strings inside binary records are **fixed-length `char` arrays**, NUL-padded.
>   `std::string`/`std::vector` are forbidden inside records. (Putting an STL container
>   into a struct and `memcpy`-ing it is undefined behavior.)
> - Money is stored as **integer cents** (`int64_t`) in binary, formatted with two
>   decimals in text. No `double` anywhere a price is persisted.
> - Crash safety: every full rewrite uses **write-temp-then-rename** (`*.tmp` →
>   `std::rename(*.tmp, *)`). Appends rely on the OS's append guarantee.

---

## 1. `data/users.dat` — BINARY

Records of all login-able users (Admin, Cashier, Kitchen).

### Record struct: `UserRecord`

```
struct UserRecord {            // size = 128 bytes
    uint32_t  id;              // 4 bytes
    char      username[24];    // 24 bytes, NUL-padded
    char      passwordHash[48];// 48 bytes  (hex of SHA-style demo hash)
    char      salt[16];        // 16 bytes
    char      fullName[32];    // 32 bytes
    uint8_t   role;            // 1 byte    0=Admin, 1=Cashier, 2=Kitchen
    uint8_t   active;          // 1 byte    1=enabled, 0=disabled
    int64_t   createdAtEpoch;  // 8 bytes
    char      _pad[?];         // pad to a stable size — final size confirmed Phase 2
};
```

> **Note.** Final struct alignment / padding is locked in Phase 2 with a
> `static_assert(sizeof(UserRecord) == N)` and `static_assert(std::is_trivially_copyable_v<UserRecord>)`.

### Default seed
On first run with a missing file, `UserRepository::seed()` writes one record:
- `username = "admin"`, `passwordHash = hash("admin123", salt)`, `role = 0`, `active = 1`.

---

## 2. `data/menu.txt` — TEXT

One menu item per line.

### Header (line 1, comment)
```
# id|name|category|priceCents|imagePath|available
```

### Line format
```
<id:uint>|<name:string>|<category:enum>|<priceCents:uint64>|<imagePath:string>|<available:0|1>
```

- `category` ∈ `{FastFood, BBQ, Chinese, Drinks, Desserts}` (exact, case-sensitive)
- `name`, `imagePath` may NOT contain `|`. Spaces are allowed.
- `imagePath` is relative to `assets/img/menu/`.

### Example
```
# id|name|category|priceCents|imagePath|available
1|Classic Cheeseburger|FastFood|799|burger.png|1
2|Grilled Chicken BBQ|BBQ|1299|bbq_chicken.png|1
3|Sweet & Sour Chicken|Chinese|1099|sweet_sour.png|1
4|Iced Latte|Drinks|449|iced_latte.png|1
5|Chocolate Lava Cake|Desserts|599|lava_cake.png|0
```

---

## 3. `data/inventory.txt` — TEXT

One ingredient per line.

### Header
```
# id|name|unit|stock|reorderThreshold|costCentsPerUnit
```

### Line format
```
<id:uint>|<name:string>|<unit:string>|<stock:double>|<reorderThreshold:double>|<costCentsPerUnit:uint64>
```

- `unit` is a free-form short string (`g`, `kg`, `ml`, `pcs`).
- `stock` and `reorderThreshold` are decimals (e.g. `12.5`).

### Example
```
# id|name|unit|stock|reorderThreshold|costCentsPerUnit
1|Beef Patty|pcs|48|10|250
2|Burger Bun|pcs|60|15|45
3|Cheese Slice|pcs|72|20|30
4|Lettuce|g|800|200|2
5|Espresso Beans|g|1500|400|8
```

---

## 4. `data/recipes.txt` — TEXT

Composition: which ingredients (and how much) a menu item consumes.

### Header
```
# menuItemId|ingredientId:qty,ingredientId:qty,...
```

### Line format
```
<menuItemId:uint>|<ingId>:<qty>,<ingId>:<qty>,...
```

### Example
```
# menuItemId|components
1|1:1,2:1,3:1,4:30
4|5:18
```

> Empty recipes are allowed but logged as a warning (the order will succeed without
> deducting stock).

---

## 5. `data/orders.dat` — BINARY (two interleaved record types)

Orders are variable-length (number of items varies), so the file is a **sequence of
(header, items[N])** pairs. Reading requires reading the header first, then `N`
item records.

### Record structs

```
struct OrderHeaderRecord {              // size = 96 bytes (pre-padding)
    uint32_t  id;                       // 4
    int64_t   placedAtEpoch;            // 8
    uint16_t  itemCount;                // 2  → tells reader how many OrderItemRecords follow
    uint8_t   status;                   // 1  0=Pending,1=Preparing,2=Ready,3=Served,4=Cancelled
    uint8_t   paymentMethod;            // 1  0=Cash,1=Card,2=OnlineSim,255=unpaid
    int64_t   discountCents;            // 8
    uint16_t  taxBp;                    // 2  basis points (e.g. 1000 = 10.00%)
    uint32_t  customerId;               // 4  0 if no customer
    char      cashierUsername[24];      // 24
    char      specialInstructions[64];  // 64
    int64_t   totalCents;               // 8  cached total at place-time
    // padding to a stable size confirmed Phase 2
};

struct OrderItemRecord {                // size = 56 bytes (pre-padding)
    uint32_t  menuItemId;               // 4
    uint16_t  qty;                      // 2
    int64_t   unitPriceCents;           // 8
    char      notes[40];                // 40
    // padding to a stable size confirmed Phase 2
};
```

### Read algorithm

```
while (more bytes) {
    read OrderHeaderRecord h;
    Order order(h);
    for (int i = 0; i < h.itemCount; ++i) {
        read OrderItemRecord ir;
        order.addRaw(ir);
    }
    out.push_back(order);
}
```

### Write algorithm (append)
Open `data/orders.dat` in `ios::binary | ios::app`, write header, then `itemCount`
item records. Status updates (e.g. Preparing → Ready) need a full rewrite via
`overwriteAll(orders)` (write-temp-then-rename) because record sizes around the
target order can change if items were edited.

---

## 6. `data/receipts/RCPT-XXXXXX.txt` — TEXT (one file per receipt)

A human-readable receipt, also serving as the "PDF" export (text-based). Filenames
are zero-padded order ids: `RCPT-000123.txt`. The exact 42-column thermal layout:

```
============================================
        SMART RESTAURANT POS
       123 Main Street, City
        Tel: +XX-XXX-XXXXXXX
============================================
Receipt #:  RCPT-000123
Order #:    123
Date:       2026-05-19 14:32:08
Cashier:    nadia
Customer:   Ali Khan (+92 300 1234567)
--------------------------------------------
Qty  Item                     Price   Total
--------------------------------------------
  2  Classic Cheeseburger     7.99   15.98
  1  Iced Latte               4.49    4.49
  1  Chocolate Lava Cake      5.99    5.99
       └ note: extra hot
--------------------------------------------
Subtotal:                            26.46
Discount:                            -2.00
Tax (10.00%):                         2.45
--------------------------------------------
TOTAL:                               26.91
Payment:    CARD  ****-1234
--------------------------------------------
Loyalty Points Earned: 26
Tier:                  Silver

       Thank you and please come again!
============================================
```

Generated by `Receipt::operator<<` (see `03-class-design.md` §G2).

---

## 7. `data/customers.dat` — BINARY

### Record struct: `CustomerRecord`

```
struct CustomerRecord {       // size = 112 bytes (pre-padding)
    uint32_t id;              // 4
    char     name[40];        // 40
    char     phone[20];       // 20
    int64_t  joinedAtEpoch;   // 8
    int64_t  totalSpentCents; // 8
    int32_t  loyaltyPoints;   // 4
    int32_t  orderCount;      // 4
    uint8_t  active;          // 1
    // padding ...
};
```

Phone numbers are stored exactly as the cashier typed them (e.g. `+92 300 1234567`)
to ease search. Repository builds an in-memory map keyed by trimmed phone on load.

---

## 8. `data/loyalty.dat` — BINARY

An append-only ledger of loyalty point transactions (useful for audit + analytics).

```
struct LoyaltyRecord {        // size = 32 bytes
    uint32_t customerId;      // 4
    uint32_t orderId;         // 4
    int64_t  atEpoch;         // 8
    int32_t  pointsDelta;     // 4  may be negative (redemption — optional feature)
    int64_t  spendCents;      // 8
    uint8_t  reason;          // 1  0=Earn, 1=Redeem, 2=Adjust
    // pad to 32
};
```

---

## 9. `data/sales_history.dat` — BINARY

A flat, append-only stream of per-order sales records, optimized for analytics scans.
Duplicates information already in `orders.dat`, intentionally — analytics queries
should not have to read the variable-length `orders.dat` to draw charts.

```
struct SalesRecord {          // size = 64 bytes
    uint32_t orderId;         // 4
    int64_t  epoch;           // 8
    int64_t  totalCents;      // 8
    int16_t  itemCount;       // 2
    uint8_t  paymentMethod;   // 1
    uint8_t  hour;            // 1   0..23, cached for fast peak-hour binning
    char     cashier[24];     // 24
    int32_t  customerId;      // 4   0 if none
    // pad to 64
};
```

> Daily/weekly/monthly reports read this file linearly and bin into
> `Report::hourBuckets` and `Report::itemCounts`. For 5,000 orders the whole file
> is ~320 KB — a trivial scan.

---

## 10. `data/tables.txt` — TEXT

### Header
```
# id|seats|status|reservedForCustomerId|reservedAtEpoch
```

### Line format
```
<id:uint>|<seats:int>|<status:Free|Occupied|Reserved>|<reservedForCustomerId:uint|0>|<reservedAtEpoch:int64|0>
```

### Example
```
# id|seats|status|reservedForCustomerId|reservedAtEpoch
1|2|Free|0|0
2|4|Occupied|0|0
3|4|Reserved|17|1747654000
4|6|Free|0|0
```

The whole file is rewritten on every state change (≤ a few dozen tables — cost is
negligible).

---

## 11. `data/settings.txt` — TEXT (key = value)

One key/value per line. `=` is the separator. `#` starts a comment.

### Example
```
# Smart POS settings
theme=Dark
rememberedUser=admin
restaurantName=Smart Restaurant
taxBp=1000
currencySymbol=$
loyaltyPointsPerDollar=1
soundEnabled=1
```

Values are read by `SettingsRepository::get(key)`; type coercion happens at the
read site (e.g. `std::stoi`). Unknown keys are ignored.

---

## 12. `data/log.txt` — TEXT (append-only)

One log line per event. Free format but conventionally:
```
<ISO8601> <LEVEL> <module> <message>
```
Example:
```
2026-05-19T14:32:08Z INFO  OrderService order#123 placed by nadia total=2691c
2026-05-19T14:35:11Z WARN  InventoryService low stock ingredient#5 stock=380 threshold=400
2026-05-19T14:39:00Z ERROR FileIO write failed path=data/orders.dat errno=13
```

Never rotated automatically in scope; manual deletion is OK for a uni demo.

---

## 13. Static asserts (Phase 2 will lock these)

In Phase 2 the persistence headers will add:

```
static_assert(std::is_trivially_copyable_v<UserRecord>);
static_assert(std::is_trivially_copyable_v<OrderHeaderRecord>);
static_assert(std::is_trivially_copyable_v<OrderItemRecord>);
static_assert(std::is_trivially_copyable_v<CustomerRecord>);
static_assert(std::is_trivially_copyable_v<LoyaltyRecord>);
static_assert(std::is_trivially_copyable_v<SalesRecord>);
static_assert(sizeof(UserRecord) == EXPECTED);   // pinned exact byte count
// ... and so on for every binary record struct.
```

This protects the on-disk format from accidental reordering / padding changes.

---

## 14. Migration / versioning

For a uni project this is **out of scope**. If the format ever changes:
- bump a `version` byte at the front of the file (planned slot reserved in each
  binary record's pad bytes), or
- ship a one-off converter as a separate `tools/migrate.cpp`.

Don't attempt schema migration in the running app.

---

*End of `08-file-schemas.md`.*
