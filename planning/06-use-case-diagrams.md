# 06 — Use-Case Diagrams (Mermaid)

> Phase 1 planning document. Mermaid has no first-class UML use-case syntax, so we use
> `flowchart LR` with circular nodes for use cases (the closest visual match) and
> `subgraph "System"` to enclose them. One diagram per actor, then a combined view.

---

## 1. Actors and their authority (summary)

| Actor | Authority |
|---|---|
| **Admin** | Full system access — manages menu, inventory, users, and views all reports. |
| **Cashier** | Front-of-house: takes orders, prints receipts, manages tables and customers. |
| **Kitchen Staff** | Sees active orders only; advances ticket status. |
| **Customer** (passive actor) | Object of customer/loyalty/table records — does not log in. |

Why "Customer" is passive: there is no customer-facing screen. The Customer entity is
managed by the Cashier on the customer's behalf.

---

## 2. Admin

```mermaid
flowchart LR
    A((Admin))
    subgraph SmartPOS["Smart Restaurant POS"]
        UC1((Login / Logout))
        UC2((Manage Users))
        UC3((Manage Menu))
        UC4((Manage Inventory))
        UC5((Manage Recipes))
        UC6((View Dashboard))
        UC7((View Analytics Reports))
        UC8((View Sales History))
        UC9((Switch Theme))
        UC10((Backup / Restore Data))
        UC11((View Kitchen Display))
        UC12((Override Order Status))
        UC13((Manage Tables))
        UC14((Place / Cancel Order))
        UC15((Issue Refund / Void Receipt))
    end
    A --> UC1
    A --> UC2
    A --> UC3
    A --> UC4
    A --> UC5
    A --> UC6
    A --> UC7
    A --> UC8
    A --> UC9
    A --> UC10
    A --> UC11
    A --> UC12
    A --> UC13
    A --> UC14
    A --> UC15
```

> Includes: every Cashier and Kitchen use case is also available to Admin. The
> diagram above shows the *extras* that only Admin can perform plus the shared
> use cases. UC10 (Backup/Restore) and UC15 (Refund) are flagged OPTIONAL in
> `01-architecture.md`.

---

## 3. Cashier

```mermaid
flowchart LR
    C((Cashier))
    subgraph SmartPOS["Smart Restaurant POS"]
        UC1((Login / Logout))
        UC2((Browse Menu))
        UC3((Place Order))
        UC4((Modify / Remove Items in Draft))
        UC5((Apply Discount))
        UC6((Add Special Instructions))
        UC7((Generate Receipt))
        UC8((Select Payment Method))
        UC9((Print / Save / PDF Receipt))
        UC10((Search Customer by Phone))
        UC11((Register New Customer))
        UC12((Apply Loyalty Points))
        UC13((Reserve Table))
        UC14((Mark Table Occupied / Free))
        UC15((View Active Orders))
        UC16((View Dashboard Snippets))
        UC17((Switch Theme))
    end
    C --> UC1
    C --> UC2
    C --> UC3
    C --> UC4
    C --> UC5
    C --> UC6
    C --> UC7
    C --> UC8
    C --> UC9
    C --> UC10
    C --> UC11
    C --> UC12
    C --> UC13
    C --> UC14
    C --> UC15
    C --> UC16
    C --> UC17

    UC3 -.->|<<include>>| UC15
    UC7 -.->|<<include>>| UC8
    UC3 -.->|<<extend>>| UC5
    UC3 -.->|<<extend>>| UC10
```

> `<<include>>` and `<<extend>>` are shown as dashed arrows — Mermaid renders them
> labeled but not as native UML stereotypes.

---

## 4. Kitchen Staff

```mermaid
flowchart LR
    K((Kitchen Staff))
    subgraph SmartPOS["Smart Restaurant POS"]
        UC1((Login / Logout))
        UC2((View Pending Orders))
        UC3((View Preparing Orders))
        UC4((View Ready Orders))
        UC5((Advance Ticket → Preparing))
        UC6((Advance Ticket → Ready))
        UC7((Mark Order Served))
        UC8((See Special Instructions))
    end
    K --> UC1
    K --> UC2
    K --> UC3
    K --> UC4
    K --> UC5
    K --> UC6
    K --> UC7
    K --> UC8

    UC2 -.->|<<include>>| UC8
    UC3 -.->|<<include>>| UC8
```

> Kitchen has **no write access** to menu, inventory, customers, tables, or analytics.
> The sidebar hides those entries based on `User::can(Capability)`.

---

## 5. System-triggered use cases (no human actor)

Some use cases run on application timers / events. They aren't triggered by a user
but they touch the same domain services, so they belong in the model.

```mermaid
flowchart LR
    SYS((System / App Tick))
    subgraph SmartPOS["Smart Restaurant POS"]
        UC1((Auto-deduct Inventory on Order Placement))
        UC2((Emit Low-Stock Notification))
        UC3((Append Sales History Record))
        UC4((Autosave Settings))
        UC5((Update Real-Time Clock))
        UC6((Animate UI Tweens))
        UC7((Persist Dirty Repositories on Shutdown))
    end
    SYS --> UC1
    SYS --> UC2
    SYS --> UC3
    SYS --> UC4
    SYS --> UC5
    SYS --> UC6
    SYS --> UC7
```

---

## 6. Combined view (all three human actors)

A small combined diagram, useful as a single slide in the report.

```mermaid
flowchart LR
    Admin((Admin))
    Cashier((Cashier))
    KitchenStaff((Kitchen Staff))

    subgraph SmartPOS["Smart Restaurant POS"]
        Login((Login / Logout))
        Menu((Manage Menu))
        Inv((Manage Inventory))
        Orders((Place Order))
        Receipt((Generate Receipt))
        Customers((Manage Customers))
        Tables((Manage Tables))
        KitchenView((Kitchen Display))
        Advance((Advance Ticket Status))
        Analytics((View Analytics))
        Users((Manage Users))
        Theme((Switch Theme))
    end

    Admin --> Login
    Admin --> Menu
    Admin --> Inv
    Admin --> Orders
    Admin --> Receipt
    Admin --> Customers
    Admin --> Tables
    Admin --> KitchenView
    Admin --> Advance
    Admin --> Analytics
    Admin --> Users
    Admin --> Theme

    Cashier --> Login
    Cashier --> Orders
    Cashier --> Receipt
    Cashier --> Customers
    Cashier --> Tables
    Cashier --> Theme

    KitchenStaff --> Login
    KitchenStaff --> KitchenView
    KitchenStaff --> Advance
```

---

## 7. Authority matrix (programmatic mirror of `User::can`)

This table mirrors what each role's `can(Capability)` method returns. The use-case
diagrams above are the human-readable face of this table.

| Capability                  | Admin | Cashier | Kitchen |
|----------------------------|:-----:|:-------:|:-------:|
| `ViewMenu`                  | ✅ | ✅ | ❌ |
| `EditMenu`                  | ✅ | ❌ | ❌ |
| `EditInventory`             | ✅ | ❌ | ❌ |
| `PlaceOrder`                | ✅ | ✅ | ❌ |
| `ManageTables`              | ✅ | ✅ | ❌ |
| `ManageCustomers`           | ✅ | ✅ | ❌ |
| `ViewKitchen`               | ✅ | ❌ | ✅ |
| `AdvanceTicket`             | ✅ | ❌ | ✅ |
| `ViewAnalytics`             | ✅ | ❌ | ❌ |
| `ManageUsers`               | ✅ | ❌ | ❌ |

---

*End of `06-use-case-diagrams.md`.*
