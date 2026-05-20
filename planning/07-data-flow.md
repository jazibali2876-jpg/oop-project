# 07 — Data Flow Diagrams (Mermaid)

> Phase 1 planning document. DFDs show how data moves between the UI layer, the
> domain layer, and the data files. Mermaid `flowchart LR/TD` is used because
> it lets us pick distinct node shapes per DFD element type:
>
> - `[external]` rectangle → **external entity** (the human at the keyboard).
> - `((process))` circle → **process** (a service method).
> - `[(data store)]` cylinder → **data store** (a file).
> - `>flow]` lozenge → **data flow label** when needed (rarely; arrows + labels usually suffice).
>
> Arrows are labelled with the data payload moving across that arrow.

---

## 1. Context-level (Level 0) — the whole system as one process

```mermaid
flowchart LR
    Admin[Admin]
    Cashier[Cashier]
    Kitchen[Kitchen Staff]

    POS(((Smart Restaurant POS)))

    Admin -- credentials, menu/inventory edits, report queries --> POS
    POS -- dashboards, reports, edit confirmations --> Admin

    Cashier -- credentials, order drafts, payments --> POS
    POS -- receipts, customer info, table states --> Cashier

    Kitchen -- credentials, ticket status updates --> POS
    POS -- active tickets, special instructions --> Kitchen
```

---

## 2. Level 1 — main subsystems

```mermaid
flowchart LR
    Admin[Admin]
    Cashier[Cashier]
    Kitchen[Kitchen]

    P1((Auth))
    P2((Menu Mgmt))
    P3((Order Processing))
    P4((Billing))
    P5((Inventory))
    P6((Customer Mgmt))
    P7((Tables))
    P8((Kitchen Display))
    P9((Analytics))

    F_users[(users.dat)]
    F_menu[(menu.txt)]
    F_inv[(inventory.txt)]
    F_rec[(recipes.txt)]
    F_orders[(orders.dat)]
    F_receipts[(receipts/*.txt)]
    F_cust[(customers.dat)]
    F_loy[(loyalty.dat)]
    F_tables[(tables.txt)]
    F_sales[(sales_history.dat)]
    F_set[(settings.txt)]

    Admin --> P1
    Cashier --> P1
    Kitchen --> P1
    P1 <--> F_users
    P1 --> F_set

    Admin --> P2
    P2 <--> F_menu

    Admin --> P5
    P5 <--> F_inv
    P5 <--> F_rec

    Cashier --> P3
    P3 -- "reads menu" --> F_menu
    P3 -- "checks stock" --> P5
    P3 -- "writes order" --> F_orders
    P3 -- "appends sales" --> F_sales
    P3 -- "enqueues ticket" --> P8

    P3 --> P4
    P4 -- "writes receipt" --> F_receipts
    P4 -- "updates loyalty" --> P6
    P6 <--> F_cust
    P6 <--> F_loy

    Cashier --> P7
    P7 <--> F_tables

    Kitchen --> P8
    P8 -- "updates status" --> F_orders

    Admin --> P9
    P9 -- "reads sales" --> F_sales
    P9 -- "reads menu" --> F_menu
```

---

## 3. Level 2 — Login flow

```mermaid
flowchart TD
    U[User at keyboard]
    UI((LoginScreen))
    A((AuthService.login))
    H((PasswordHash.verify))
    UR((UserRepository))
    F[(users.dat)]
    S((Session))

    U -- "username + password" --> UI
    UI -- "AuthService.login(u, p)" --> A
    A -- "all()" --> UR
    UR -- "binary read" --> F
    F -- "UserRecord[]" --> UR
    UR -- "vector<UserRecord>" --> A
    A -- "(stored hash, salt, attempt)" --> H
    H -- "match? bool" --> A
    A -- "unique_ptr<User>" --> S
    S -- "isAuthenticated()" --> UI
    UI -- "navigate to Dashboard" --> UI
```

Failure path: if `H` returns false, `A` throws `InvalidCredentialsException`. The
`UI` catches it and pushes a red `Toast`; `Session` stays cleared.

---

## 4. Level 2 — Place Order flow (the central use case)

```mermaid
flowchart TD
    C[Cashier]
    OS((OrderScreen))
    DR>Draft Order in UI state]
    OSV((OrderService.place))
    INV((InventoryService.canFulfill / deduct))
    KIT((KitchenService.enqueue))
    OREP((OrderRepository))
    SREP((SalesHistoryRepository))
    F_orders[(orders.dat)]
    F_sales[(sales_history.dat)]
    F_inv[(inventory.txt)]
    BIL((BillingService.makeReceipt))
    RREP((ReceiptRepository))
    F_rcpt[(receipts/RCPT-XXXX.txt)]
    BSCR((BillingScreen))

    C -- "tap menu items" --> OS
    OS -- "add to draft" --> DR
    C -- "tap Place Order" --> OS
    OS -- "OrderService.place(draft, session.user, customerId?)" --> OSV
    OSV -- "canFulfill?" --> INV
    INV -- "reads stock" --> F_inv
    INV -- "true / false" --> OSV
    OSV -- "deduct(order)" --> INV
    INV -- "writes updated stock" --> F_inv
    OSV -- "append(order)" --> OREP
    OREP -- "binary append" --> F_orders
    OSV -- "append(SalesRecord)" --> SREP
    SREP -- "binary append" --> F_sales
    OSV -- "enqueue(order)" --> KIT
    OSV -- "return Order" --> OS
    OS -- "makeReceipt(order)" --> BIL
    BIL -- "Receipt" --> OS
    OS -- "navigate" --> BSCR
    BSCR -- "saveReceiptFile(receipt)" --> RREP
    RREP -- "text write" --> F_rcpt
```

Error paths (each pushes a toast):
- empty draft → `EmptyOrderException`
- `canFulfill` returns false → `InsufficientStockException` (with which ingredient)
- file write error → `FileIOException`

---

## 5. Level 2 — Generate Analytics Report flow

```mermaid
flowchart TD
    A[Admin]
    AS((AnalyticsScreen))
    AF>AnalyticsFilter: from, to, grouping]
    ASV((AnalyticsService.buildReport))
    SREP((SalesHistoryRepository))
    MREP((MenuRepository))
    F_sales[(sales_history.dat)]
    F_menu[(menu.txt)]
    R>Report: revenue, orderCount, itemCounts, hourBuckets]
    CH((widgets::BarChart / PieChart / LineChart))

    A -- "pick date range" --> AS
    AS -- "filter values" --> AF
    AS -- "buildReport(from, to)" --> ASV
    ASV -- "all()" --> SREP
    SREP -- "binary read" --> F_sales
    F_sales -- "SalesRecord[]" --> SREP
    SREP -- "vector<SalesRecord>" --> ASV
    ASV -- "byId for top items" --> MREP
    MREP -- "text read" --> F_menu
    F_menu -- "MenuItem[]" --> MREP
    MREP -- "MenuItem" --> ASV
    ASV -- "Report" --> R
    R -- "topItems(5)" --> AS
    AS -- "draw bars" --> CH
    AS -- "draw pie" --> CH
    AS -- "draw revenue line" --> CH
    CH -- "ImDrawList primitives" --> AS
```

Note: charts are drawn **inside the same frame** the screen runs — there's no
intermediate file. The `Report` is held in transient UI state until the user
changes the filter.

---

## 6. Level 2 — Manage Menu (CRUD)

```mermaid
flowchart TD
    A[Admin]
    MS((MenuScreen))
    MSV((MenuService))
    MREP((MenuRepository))
    F[(menu.txt)]

    A -- "Add Item: name, price, category, image" --> MS
    MS -- "MenuService.add(item)" --> MSV
    MSV -- "duplicate id?" --> MSV
    MSV -- "items.push_back" --> MSV
    MSV -- "persist()" --> MREP
    MREP -- "write-temp-then-rename" --> F

    A -- "Edit Item" --> MS
    MS -- "MenuService.update(item)" --> MSV
    MSV -- "find & replace" --> MSV
    MSV -- "persist()" --> MREP

    A -- "Delete Item" --> MS
    MS -- "MenuService.remove(id)" --> MSV
    MSV -- "items.erase" --> MSV
    MSV -- "persist()" --> MREP

    A -- "Search / Filter" --> MS
    MS -- "MenuService.filter(cat, q)" --> MSV
    MSV -- "items[] (cached)" --> MSV
    MSV -- "vector<MenuItem>" --> MS
```

Why `persist()` after every mutation: the menu file is small, so we re-write the
whole file on each change. Crash-safety is the write-temp-then-rename pattern.

---

## 7. Level 2 — Inventory deduction (system-triggered)

```mermaid
flowchart TD
    OSV((OrderService.place))
    INV((InventoryService.deduct))
    RREP((RecipeRepository))
    F_rec[(recipes.txt)]
    IREP((InventoryRepository))
    F_inv[(inventory.txt)]
    LOG((Log low-stock toast))

    OSV -- "deduct(order)" --> INV
    INV -- "byMenuItem(id)" --> RREP
    RREP -- "text read" --> F_rec
    F_rec -- "Recipe[]" --> RREP
    RREP -- "Recipe" --> INV
    INV -- "stock -= qty * component.qty" --> INV
    INV -- "persist()" --> IREP
    IREP -- "write-temp-then-rename" --> F_inv
    INV -- "ingredients with stock <= threshold" --> LOG
```

---

## 8. Level 2 — Customer recognition & loyalty

```mermaid
flowchart TD
    Cash[Cashier]
    OSCR((OrderScreen → customer phone box))
    CSV((CustomerService.findByPhoneOrCreate))
    CREP((CustomerRepository))
    LSV((LoyaltyRule.pointsFor))
    F_cust[(customers.dat)]
    F_loy[(loyalty.dat)]
    LREP((LoyaltyRepository))

    Cash -- "enter phone" --> OSCR
    OSCR -- "findByPhoneOrCreate(phone)" --> CSV
    CSV -- "all() / find" --> CREP
    CREP -- "binary read" --> F_cust
    CSV -- "Customer (existing or new)" --> OSCR

    OSCR -- "after place: applyLoyalty(id, total)" --> CSV
    CSV -- "pointsFor(total)" --> LSV
    LSV -- "points int" --> CSV
    CSV -- "Customer.points += / Customer.totalSpent +=" --> CSV
    CSV -- "overwriteAll(customers)" --> CREP
    CREP -- "binary write" --> F_cust
    CSV -- "append LoyaltyRecord" --> LREP
    LREP -- "binary append" --> F_loy
```

---

## 9. Level 2 — Kitchen ticket advance

```mermaid
flowchart TD
    K[Kitchen Staff]
    KSCR((KitchenScreen))
    KSV((KitchenService.advance))
    OSV((OrderService.setStatus))
    OREP((OrderRepository))
    F_orders[(orders.dat)]

    K -- "tap ticket → next status" --> KSCR
    KSCR -- "advance(orderId)" --> KSV
    KSV -- "next status" --> OSV
    OSV -- "rewrite order header" --> OREP
    OREP -- "overwriteAll(orders)" --> F_orders
    KSCR -- "remove from lane if Ready→Served" --> KSCR
```

---

## 10. Cross-cutting: where files are touched (matrix)

| File | Read by | Written by |
|---|---|---|
| `users.dat` | `AuthService` | `AuthService` (addUser, password change) |
| `menu.txt` | `MenuService`, `OrderService` (resolve item names), `AnalyticsService` | `MenuService.persist` |
| `inventory.txt` | `InventoryService.canFulfill`, dashboard low-stock | `InventoryService.deduct`, `InventoryService.adjust` |
| `recipes.txt` | `InventoryService.deduct` | `InventoryService` (recipe editor — optional) |
| `orders.dat` | `OrderService.active/byId`, `AnalyticsService` (joins via id) | `OrderService.place / setStatus / cancel` |
| `receipts/*.txt` | nothing (write-only artifact) | `BillingService.saveReceiptFile` |
| `customers.dat` | `CustomerService` | `CustomerService.persist` |
| `loyalty.dat` | `AnalyticsService` (optional), `CustomerService.history` | `CustomerService.applyLoyalty` |
| `tables.txt` | `TableService.all` | `TableService.reserve/occupy/free` |
| `sales_history.dat` | `AnalyticsService.buildReport` | `OrderService.place` |
| `settings.txt` | `App` on boot, `Theme.apply` | `SettingsRepository.put` |
| `log.txt` | nothing | `util::Log` |

---

*End of `07-data-flow.md`.*
