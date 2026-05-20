# Smart Restaurant POS — Final Project Report

> **A Modern Desktop Point-of-Sale Application**
> Final-Year Object-Oriented Programming Project
>
> | | |
> |---|---|
> | **Submitted by** | Jazib Ali |
> | **Enrollment No.** | 01-135221-020 |
> | **Course** | Object Oriented Programming |
> | **Year** | 2026 |
> | **Repository** | [github.com/jazibali2876-jpg/oop-project](https://github.com/jazibali2876-jpg/oop-project) |
>
> Built with C++17 · SFML 3.1 · Dear ImGui · MinGW-w64 · CMake

The full formatted report is in [`Project_Report.docx`](Project_Report.docx). This markdown copy mirrors the same content for browsing on GitHub.

---

## Abstract

Smart Restaurant POS is an offline-first desktop point-of-sale application built as a final-year Object-Oriented Programming project. It implements the day-to-day workflow of an independent restaurant — order taking, billing and receipt printing, inventory tracking, table management, customer loyalty, a kitchen display, and analytics — in a single C++17 executable that requires no installation and no database.

The application is written in C++17 and built with the MinGW-w64 GCC toolchain. The graphical interface is rendered using Dear ImGui's immediate-mode UI on top of an SFML 3.1 window, with the integration between the two libraries hand-written from scratch. All persistent data — users, menu, inventory, orders, customers, loyalty, sales history, tables, settings, and receipts — is stored exclusively through C++ file handling, mixing human-readable text files with fixed-width binary record structs. There is no SQL, no embedded database, and no network dependency.

Beyond the functional product, the project is structured to demonstrate the full set of OOP concepts required for the course — classes, encapsulation, inheritance, polymorphism, abstraction, virtual functions, templates, operator overloading, exception handling, the C++ STL, modular programming, and header/source separation. Each concept is grounded in a specific named class so it can be defended directly during the viva.

---

## Table of Contents

1. [Introduction](#1--introduction)
2. [Objectives](#2--objectives)
3. [Existing Systems & Motivation](#3--existing-systems--motivation)
4. [System Requirements](#4--system-requirements)
5. [Technology Stack](#5--technology-stack)
6. [System Architecture](#6--system-architecture)
7. [Object-Oriented Design](#7--object-oriented-design)
8. [UML & Use-Case Diagrams](#8--uml--use-case-diagrams)
9. [File-Handling Design](#9--file-handling-design)
10. [Feature Walkthrough](#10--feature-walkthrough)
11. [Implementation Highlights](#11--implementation-highlights)
12. [Testing & Verification](#12--testing--verification)
13. [Results](#13--results)
14. [Project Journey & Lessons Learned](#14--project-journey--lessons-learned)
15. [Limitations & Future Work](#15--limitations--future-work)
16. [Conclusion](#16--conclusion)
17. [References](#17--references)
- [Appendix A — Repository Layout](#appendix-a--repository-layout)
- [Appendix B — Binary File Schemas](#appendix-b--binary-file-schemas)
- [Appendix C — How to Run & Build](#appendix-c--how-to-run--build)

---

## 1.  Introduction

Restaurants — particularly independent and small-chain establishments — still juggle paper order pads, separate kitchen tickets, and standalone receipt printers. Inventory drifts out of sync with what is actually being sold. Reporting is manual. Customer loyalty data is lost. The commercial alternatives (Toast, Square, Lightspeed, Clover) solve these problems, but they are expensive monthly subscriptions, require a constant cloud connection, and force a specific hardware ecosystem.

Smart Restaurant POS is a pragmatic alternative: a single Windows executable that captures the essentials of a real POS — orders, billing, inventory, tables, customers, kitchen display, and reporting — and runs offline, without any server, database, or external dependency.

Just as importantly, this is a final-year university project for the Object-Oriented Programming course. Every architectural decision in the system was made to satisfy two constraints simultaneously: produce a real product that feels like commercial POS software, and demonstrate every OOP concept the course examines, in a way the author can defend in viva.

---

## 2.  Objectives

- **Build a functional desktop POS application** covering the core workflow: authentication, menu management, order taking, billing and receipts, inventory with auto-deduction, table reservations, a kitchen display, customer profiles with loyalty, and analytics.
- **Demonstrate the complete set of OOP concepts** — classes and objects, encapsulation, inheritance, polymorphism, abstraction, virtual functions, templates, operator overloading, exception handling, STL containers, modular programming, and header/source separation — each mapped to a specific class.
- **Implement complete file handling using only the C++ standard library** (`ifstream` / `ofstream` / `fstream`). The persistence layer must cover both human-readable text files (menu, inventory, recipes, tables, settings) and fixed-width binary record structs (users, customers, orders, loyalty ledger, sales history). No SQL, no embedded database.
- **Keep the project realistically buildable within one semester.** No enterprise frameworks, no commercial libraries, no cloud, no networking, no machine learning.

---

## 3.  Existing Systems & Motivation

Three commercial POS systems were used as visual and functional reference points: **Toast**, **Square for Restaurants**, and **Lightspeed Restaurant**. The shared traits the project aims to match are:

- A persistent left-side navigation bar with a fixed list of major areas.
- A top bar showing screen title, real-time clock, theme toggle, and signed-in user.
- Card-based dashboards with at-a-glance KPI tiles.
- Receipt previews rendered in a fixed-width thermal layout.
- Color-coded table grids that respond to one-tap state transitions.
- Three-lane kitchen displays (pending → preparing → ready).

Smart Restaurant POS does not aim to compete with those systems on scale or features. It aims to feel familiar to anyone who has used one, while being something a single developer can build, defend, and ship in a semester as a course project.

---

## 4.  System Requirements

### 4.1  Functional Requirements

| # | Requirement |
|---|---|
| FR-1  | Authenticate users via username and password, with masked input and a "remember me" option. |
| FR-2  | Support three roles — Admin, Cashier, Kitchen — each with a distinct capability set. |
| FR-3  | Admin can create, update, and delete menu items, categorized into FastFood / BBQ / Chinese / Drinks / Desserts. |
| FR-4  | Cashier can build an order, modify quantities, apply a discount, attach special instructions, and place the order. |
| FR-5  | On order placement, validate available stock against the recipe and either reject or deduct ingredients. |
| FR-6  | Generate an itemized receipt with subtotal, discount, tax, total, and payment method, in a thermal-style layout. |
| FR-7  | Track per-customer profiles (name, phone, total spent, loyalty points, tier) and apply loyalty points automatically. |
| FR-8  | Expose a table grid with Free / Occupied / Reserved states and one-tap transitions. |
| FR-9  | Kitchen role sees active orders in three lanes (Pending / Preparing / Ready) and advances their status. |
| FR-10 | Provide analytics over today / 7 days / 30 days, including revenue, order count, AOV, top items, hour-of-day distribution. |
| FR-11 | All persistent data survives a clean shutdown and is restored on the next launch. |
| FR-12 | On a fresh installation with no data files, seed sensible defaults so the app is usable immediately. |

### 4.2  Non-Functional Requirements

| # | Requirement |
|---|---|
| NFR-1 | Run on 64-bit Windows 10 or 11 with no installer, no admin rights, no network. |
| NFR-2 | Start to a usable login screen within 3 seconds on commodity hardware. |
| NFR-3 | No persistence layer shall require a database engine. Only file handling via the C++ standard library. |
| NFR-4 | Whole-file rewrites shall be crash-safe (no corrupted file if force-killed mid-write). |
| NFR-5 | All binary record structs shall have a `static_assert` guard pinning `sizeof(record)` at a stable byte count. |
| NFR-6 | The build shall be reproducible — fresh checkout + supplied toolchain produces a byte-equivalent executable. |
| NFR-7 | All third-party assets (fonts, vendored libraries) supplied locally; no runtime downloads. |
| NFR-8 | The packaged `dist/` folder shall be self-contained; every required runtime DLL sits beside the executable. |

---

## 5.  Technology Stack

| | |
|---|---|
| **Language** | C++17 (ISO/IEC 14882:2017) |
| **Compiler** | MinGW-w64 GCC 14.2.0 (UCRT / POSIX / SEH) |
| **Build system** | CMake 3.30 (vendored portable) |
| **Windowing & I/O** | SFML 3.1.0 — `sf::RenderWindow`, `sf::Event`, `sf::Texture`, `sf::Sound` |
| **UI framework** | Dear ImGui (master branch) — immediate-mode |
| **UI renderer** | `imgui_impl_opengl3` (bundled with Dear ImGui), OpenGL 3.3 |
| **Integration** | Hand-written ImGui ↔ SFML 3 glue (no third-party binding) |
| **Charts** | `ImDrawList` primitives (`AddRectFilled`, `PathArcTo`, etc.) — no ImPlot |
| **Storage** | Plain C++ file handling: `<fstream>`, text + binary |
| **Fonts** | Inter Regular / Bold + Font Awesome 6 Free (vendored TTFs) |

Every dependency is vendored inside the project folder. No third-party library is fetched at runtime.

---

## 6.  System Architecture

### 6.1  The Domain / UI Split

Dear ImGui is an immediate-mode UI library — no persistent widget objects, no `.ui` files, no signals/slots. Without discipline this encourages business logic to bleed into draw functions, which would undermine the OOP demonstration. The codebase is therefore split into two non-overlapping layers:

| Layer | Folder | Responsibility |
|---|---|---|
| **Domain** | `src/domain/` | Plain C++17 entities, services, repositories. Owns ALL business state and all persisted data. Holds the entire OOP demonstration. Includes no ImGui and no SFML headers. |
| **UI** | `src/ui/` | Immediate-mode screens, widgets, theme, animation helpers. Holds only transient UI state. Reads from the domain and calls into its services. Never opens a file directly. |
| **Platform** | `src/platform/` | Hand-written ImGui ↔ SFML 3 integration. The ONLY folder that includes both ImGui and SFML headers. |
| **Util** | `src/util/` | Tiny cross-cutting helpers — atomic file write, string trimming, logging, deterministic random. |

### 6.2  The Manual ImGui ↔ SFML 3 Integration

There is no ready-made ImGui-SFML binding for SFML 3.x — the existing one targets the older SFML 2.x API. This project writes the integration from scratch in four small classes:

- **`Platform`** — owns `sf::RenderWindow` with an attached OpenGL context, drains the event queue each frame.
- **`ImGuiBackend`** — wraps `ImGui::CreateContext`, `ImGui_ImplOpenGL3_Init`, the per-frame `NewFrame` / `Render` / `RenderDrawData` lifecycle, and shutdown.
- **`InputBridge`** — translates each SFML 3 event subtype (`event.getIf<sf::Event::MouseMoved>()`, etc.) into the corresponding `ImGuiIO::Add*Event` call.
- **`KeyMap`** — maps every scoped `sf::Keyboard::Key` value into the matching `ImGuiKey` enum.

The per-frame loop in `App::run()` is therefore explicit:

```cpp
while (platform.pumpEvents(input)) {
    float dt = platform.deltaSeconds();
    auto fb  = platform.framebufferSize();
    imgui.newFrame(dt, fb.x, fb.y);

    ui::Shell::draw(ctx);             // domain reads, ImGui draw calls
    toasts.tickAndDraw(dt, fb.x);

    platform.clear();
    imgui.render(platform.window());
    platform.display();
}
```

### 6.3  How Data Flows — Place an Order

1. `OrderScreen` draws the menu grid by reading `MenuService::all()`. Tapping an item appends it to `OrderDraft` (transient UI struct, not yet a domain `Order`).
2. "Place Order" → `OrderService::place(draft, session.user(), customerId)`.
3. `OrderService` validates (throws `EmptyOrderException` if empty), checks stock via `InventoryService::canFulfill` / `deduct` (throws `InsufficientStockException`), persists the order via `OrderRepository`, appends a `SalesRecord`, and enqueues a kitchen ticket.
4. On exception, `OrderScreen` catches `DomainException&` and pushes a red `Toast`. On success it navigates to `BillingScreen`, which builds a `Receipt` and writes it to `data/receipts/RCPT-NNNNNN.txt`.

---

## 7.  Object-Oriented Design

Every required OOP concept maps to a specific named class. This is the primary viva-defense reference.

| Concept | Where to point | What to say |
|---|---|---|
| **Classes & objects** | [`src/domain/menu/MenuItem.h`](../src/domain/menu/MenuItem.h) | Every persisted entity has its own class with private fields and behavior methods. |
| **Encapsulation** | [`src/domain/order/Order.h`](../src/domain/order/Order.h) | `Order` keeps its items vector private; mutation only via `add`, `removeAt`, `setStatus`. |
| **Inheritance** | [`src/domain/auth/User.h`](../src/domain/auth/User.h) + `Admin.h` / `Cashier.h` / `Kitchen.h` | Three concrete roles inherit from an abstract `User`. |
| **Polymorphism** | `Session::user().can(Capability)` | Sidebar asks the runtime user object; behavior differs per role. |
| **Abstraction** | `User` — pure virtual `can()` and `roleName()` | `User` cannot be instantiated. |
| **Virtual functions** | `User::can(Capability) const = 0` + `override` in each subclass | Pure-virtual base + every subclass marked `override`. |
| **Templates** | [`src/domain/persistence/BinaryRepository.h`](../src/domain/persistence/BinaryRepository.h) | One templated repo serves 4 record types; `Id<Tag>` is a templated strong-id. |
| **Operator overloading** | [`src/domain/common/Money.h`](../src/domain/common/Money.h), `Receipt::operator<<` | `Money` has `+`, `-`, `*`, comparison, stream insertion. Receipt's `operator<<` emits the thermal-style layout. |
| **Exception handling** | [`src/domain/common/Exceptions.h`](../src/domain/common/Exceptions.h) | `DomainException` base + 8 named subclasses; services throw, UI catches → toast. |
| **STL containers** | `vector`, `map`, `unordered_map`, `array<int,24>`, `optional`, `deque`, `unique_ptr` | At least six container types across the domain. |
| **Modular programming** | `src/domain/` split into 10 sub-packages | + separate `ui/`, `platform/`, `util/` layers. |
| **Header/source split** | Every non-template class | Templates stay in headers by language necessity. |

**Volunteer-in-viva concepts** (not required but present): RAII (Platform, ImGuiBackend), smart pointers (`unique_ptr<User>`), const-correctness, move semantics, `std::optional`, lambdas inside `std::find_if` / `std::remove_if`.

---

## 8.  UML & Use-Case Diagrams

Full Mermaid sources live in [`planning/05-uml-diagrams.md`](../planning/05-uml-diagrams.md) and [`planning/06-use-case-diagrams.md`](../planning/06-use-case-diagrams.md). They render directly on GitHub.

**8.1 Auth.** `User` is abstract; `Admin` / `Cashier` / `Kitchen` inherit and override `can(Capability)` and `roleName()`. `Session` owns the current user via `std::unique_ptr<User>`. `AuthService` constructs users either by reading via `UserRepository` or by registering with a salted hash via `PasswordHash`.

**8.2 Order & Billing.** `Order` aggregates `OrderItem` instances and computes subtotal, tax, total, and prep time. `OrderService` orchestrates against `OrderRepository`, `InventoryService`, `KitchenService`, and `SalesHistoryRepository`. `BillingService` consumes an `Order` and produces a `Receipt`, whose `operator<<` emits the printable thermal-style text.

**8.3 Persistence (templates in action).** `TextRepository` is the base for line-oriented text storage. `BinaryRepository<T>` is the templated base for fixed-width POD records, instantiated for `UserRecord`, `CustomerRecord`, `LoyaltyRecord`, `SalesRecord`. `OrderRepository` is a special case — it writes variable-length records (one header + N item rows) so it manages the stream itself.

**8.4 Use cases.**

| Actor | Permitted use cases |
|---|---|
| **Admin**   | Everything Cashier and Kitchen can do, plus: Manage Menu, Manage Inventory, Manage Recipes, Manage Users, View Analytics, Switch Theme, Override Order Status. |
| **Cashier** | Login/Logout, Browse Menu, Place Order, Modify Draft, Apply Discount, Generate Receipt, Select Payment Method, Save/Export Receipt, Lookup/Register Customer, Apply Loyalty, Reserve Table, Mark Table Occupied/Free. |
| **Kitchen** | Login/Logout, View Pending/Preparing/Ready lanes, Advance Ticket Status, See Special Instructions. |

**8.5 Place-Order data flow.**

```
Cashier → OrderScreen → OrderDraft (UI state)
              ↓ place()
          OrderService
             ├── InventoryService::canFulfill / deduct  → inventory.txt
             ├── OrderRepository::append                 → orders.dat (binary)
             ├── SalesHistoryRepository::append          → sales_history.dat
             └── KitchenService::enqueue                 → (in-memory queue)
          BillingService::makeReceipt
              ↓
          ReceiptRepository::write                       → receipts/RCPT-NNNNNN.txt
              ↓
          BillingScreen (preview + payment)
```

---

## 9.  File-Handling Design

All persistence uses the C++ standard library: `std::ifstream`, `std::ofstream`, `std::fstream`, `std::filesystem`. No SQL, no embedded database engine.

### 9.1  Text Files

| File | Contents |
|---|---|
| `data/menu.txt`         | `id \| name \| category \| priceCents \| imagePath \| available` |
| `data/inventory.txt`    | `id \| name \| unit \| stock \| reorderThreshold \| costCentsPerUnit` |
| `data/recipes.txt`      | `menuItemId \| ingredientId:qty, ingredientId:qty, ...` |
| `data/tables.txt`       | `id \| seats \| status \| reservedForCustomerId \| reservedAtEpoch` |
| `data/settings.txt`     | `k=v` lines — theme, rememberedUser, taxBp, currencySymbol, ... |
| `data/log.txt`          | ISO8601 timestamp + level + module + message |
| `data/receipts/RCPT-NNNNNN.txt` | Thermal-style receipt — output of `Receipt::operator<<` |

### 9.2  Binary Record Files

| File | Record | Size | Notes |
|---|---|---|---|
| `data/users.dat`         | `UserRecord`               | 128 B | 1 record per login-able user. |
| `data/orders.dat`        | `OrderHeader + N×OrderItem`| 128 + 56·N B | Variable-length. `itemCount` in the header. |
| `data/customers.dat`     | `CustomerRecord`           | 96 B  | Phone, name, lifetime spend, loyalty points. |
| `data/loyalty.dat`       | `LoyaltyRecord`            | 32 B  | Append-only ledger. |
| `data/sales_history.dat` | `SalesRecord`              | 64 B  | Flat scan for analytics. |

Each record struct uses `#pragma pack(1)` + `static_assert(sizeof(R) == N)` so the on-disk format cannot drift silently. Full layouts are in [Appendix B](#appendix-b--binary-file-schemas).

### 9.3  Crash Safety

Whole-file rewrites use **write-temp-then-rename**: write to `<path>.tmp`, then `std::filesystem::rename` atomically replaces the live file. A power loss either leaves the original file untouched or installs the new one cleanly. Centralized in `pos::util::writeAtomic` (`src/util/Files.cpp`).

### 9.4  First-Run Behavior

On a fresh install with no data files, the app:

1. Creates `data/` and `data/receipts/` (`util::mkdirs`).
2. Seeds three demo users (admin / cashier / kitchen) via `AuthService::seedDemoUsersIfEmpty`.
3. Seeds 5 menu items, 5 ingredients, 2 recipes, 10 tables.
4. Every repository tolerates missing files as "empty content" — never throws. You can delete `data/` at any time to reset; the next launch re-seeds.

---

## 10.  Feature Walkthrough

**10.1 Splash & Login.** 1.6-second splash fades into the login screen. Masked password input, "remember me" persists username in `settings.txt`, three demo accounts listed as a tip. Invalid credentials → 0.4-second shake animation + red toast.

**10.2 Dashboard.** Four metric cards (today's revenue, active orders, low-stock count, top item), top-items list, hand-drawn 7-day revenue line chart, low-stock alert table.

**10.3 Menu Management.** Category filter + search + grid of cards. Admin sees Add/Edit/Delete; cashier/kitchen see read-only. Add/edit opens a modal.

**10.4 New Order.** Menu grid on the left, cart on the right. Tapping a card adds to `OrderDraft`. Live subtotal/discount/tax/total + estimated prep time. Phone lookup attaches a customer.

**10.5 Billing & Receipts.** Payment method selection, live receipt preview via `Receipt::operator<<`, Save / Export PDF-style buttons. Charging records payment and applies loyalty.

**10.6 Inventory.** Table of ingredients; low-stock rows tinted amber. Auto-deduction fires on every order placement via recipes.

**10.7 Tables.** Color-coded grid (green Free, amber Occupied, blue Reserved). Tap to reserve / mark occupied / free. Reservation attaches a Customer.

**10.8 Kitchen Display.** Three lanes (Pending / Preparing / Ready) with live elapsed time per ticket. One-tap to advance status. "Mark Served" removes from queue.

**10.9 Analytics.** Today / 7 days / 30 days range. Hand-drawn bar chart (revenue per day), pie chart (top items), and line chart (orders by hour of day) — all via `ImDrawList`, no ImPlot.

---

## 11.  Implementation Highlights

### 11.1  Role Hierarchy (polymorphism)

```cpp
class User {
public:
    virtual ~User() = default;
    virtual bool        can(Capability) const = 0;
    virtual std::string roleName() const = 0;
};

class Cashier final : public User {
public:
    bool can(Capability c) const override {
        switch (c) {
            case Capability::ViewMenu:
            case Capability::PlaceOrder:
            case Capability::ManageTables:
            case Capability::ManageCustomers:
                return true;
            default:
                return false;
        }
    }
    std::string roleName() const override { return "Cashier"; }
};
```

### 11.2  Templated Repository

```cpp
template <class T>
class BinaryRepository {
public:
    static_assert(std::is_trivially_copyable_v<T>,
                  "BinaryRepository<T>: T must be trivially copyable");

    std::vector<T> all() const;
    void           append(const T& rec);
    void           overwriteAll(const std::vector<T>& v);

    template <class Pred>
    std::optional<T> findIf(Pred p) const;
};

class UserRepository : public BinaryRepository<UserRecord> { /* ... */ };
```

### 11.3  Operator Overloading on `Money`

```cpp
class Money {
public:
    Money& operator+=(const Money& r) { cents_ += r.cents_; return *this; }
    friend Money operator+(Money a, const Money& b) { a += b; return a; }
    friend Money operator*(Money a, int n)          { a *= n; return a; }
    friend bool  operator<(const Money& a, const Money& b)
                                          { return a.cents_ < b.cents_; }
    friend std::ostream& operator<<(std::ostream& os, const Money& m);
private:
    std::int64_t cents_ = 0;
};
```

### 11.4  Exception Boundary

```cpp
class DomainException : public std::runtime_error { /* ... */ };
class InsufficientStockException : public DomainException {
public:
    InsufficientStockException(unsigned ingId, double needed, double have);
    unsigned ingredientId() const;
};

// At the UI boundary (OrderScreen):
try {
    auto placed = ctx.orders.place(draft, ctx.session.user(), customerId);
    ctx.toasts.success("Order placed", "#" + std::to_string(placed.id().value()));
} catch (const DomainException& e) {
    ctx.toasts.danger("Order failed", e.what());
}
```

### 11.5  Hand-Drawn Pie Chart (no ImPlot)

```cpp
for (std::size_t i = 0; i < slices.size(); ++i) {
    float endAngle = startAngle + frac * 6.2831853f;
    dl->PathClear();
    dl->PathLineTo(center);
    int steps = std::max(1, (int)(segPerSlice * frac));
    for (int k = 0; k <= steps; ++k) {
        float a = startAngle + (endAngle - startAngle) * (k / (float)steps);
        dl->PathLineTo(ImVec2(center.x + cos(a)*radius,
                              center.y + sin(a)*radius));
    }
    dl->PathFillConvex(col);
    startAngle = endAngle;
}
```

---

## 12.  Testing & Verification

### 12.1  Compile-Time Checks
- Every binary record has `static_assert(sizeof(R) == N)` and `static_assert(std::is_trivially_copyable_v<R>)`.
- `BinaryRepository<T>` constrains `T` to trivially copyable at class scope.
- Clean compile on `-std=c++17` with no deprecation warnings under MinGW-w64 GCC 14.2.

### 12.2  Preflight Verification Script
`tools/preflight_check.ps1` runs 19 checks: compiler, SFML link probe, 7 runtime DLLs beside the exe, 3 fonts (source + packaged), and 4 asset/data paths. Must report all-PASS before later stages.

### 12.3  Fresh-Machine Simulation
Before declaring the bundle self-contained: copy `dist/` to `%TEMP%`, strip `PATH` to only `C:\Windows\System32`, launch `smart_pos.exe`. Confirms no hidden dependencies on the developer's PATH.

### 12.4  Manual Exercise Across Roles
Admin adds/edits/deletes a menu item; cashier places an order, attaches a customer, prints a receipt; kitchen advances the ticket through Preparing → Ready → Served; restart the app and confirm everything persists.

### 12.5  Crash Recovery
Force-killing the app mid-write does not leave any data file half-written in place — write-temp-then-rename guarantees the live file stays consistent.

---

## 13.  Results

| | |
|---|---|
| **Total source files (C++)** | 139 — domain (89), ui (28), platform (9), util (7), main + App |
| **Lines of C++ (estimate)** | ~6,500 hand-written |
| **Domain sub-packages** | 10 + persistence |
| **Binary record types** | 6 (UserRecord, OrderHeader, OrderItem, CustomerRecord, LoyaltyRecord, SalesRecord) |
| **Text file types** | 6 + per-receipt files |
| **Demo user accounts** | 3 (admin / cashier / kitchen) auto-seeded |
| **Built executable size** | ~2.0 MB |
| **Packaged `dist/` size** | ~14 MB |
| **Cold-start time** | Under 2 seconds on commodity hardware |
| **Build dependencies** | Vendored — MinGW, SFML source, ImGui, CMake portable, fonts |
| **Runtime dependencies** | Zero — offline on any 64-bit Windows 10/11 |

---

## 14.  Project Journey & Lessons Learned

Five gated phases, each ending in verification before the next began:

| Phase | Output |
|---|---|
| 1. Planning                       | Ten design documents (`planning/01..10`). |
| 2-Stage 1. Toolchain & smoke test | Vendored CMake; rebuilt SFML 3.1.0 with MinGW; vendored fonts; minimal ImGui-in-SFML smoke test. |
| 2-Stage 2. Domain layer           | All 89 domain files implemented; zero ImGui/SFML dependency. |
| 2-Stage 3 & 4. UI                 | Theme, widgets, animation, all ten screens; App class wires it all. |
| 2-Stage 5. Packaging              | Release build + every required runtime DLL + assets into `dist/`. Fresh-machine simulation passed. |
| Presentation & publish            | 14-slide PowerPoint via python-pptx; repo init, gitignore tuned, pushed to GitHub. |

### 14.1  Real Engineering Problems Encountered

- **SFML toolchain mismatch.** Pre-supplied SFML 3.1.0 was an MSVC build (`.lib` + `.pdb`). MinGW can't link MSVC C++ libraries because the C++ ABI differs. **Fix:** rebuilt SFML 3.1.0 from source with the bundled MinGW into `third_party/SFML-mingw/`, producing proper `lib*.a` archives.
- **Password hash truncation.** First commit's login with admin/admin123 failed. Diagnosis traced it to a `copyFixed` helper that reserved the last byte of every fixed-width field for a NUL terminator — silently truncating the 32-char password hash to 31. The on-disk hash could never re-verify. **Fix:** `copyFixed` now writes up to `N` bytes (zero-init first, so shorter strings still get an implicit terminator). Verified via `tools/probes/login_probe.cpp`.
- **Demo-user coverage.** Original seed only created admin; demoing other roles required Admin to create them first. **Fix:** `seedDemoUsersIfEmpty` idempotently creates all three demo accounts on first run.

### 14.2  What the Project Taught
- Immediate-mode UI and rigorous OOP can coexist, but only with an explicit, enforced separation between the domain layer (object-oriented) and the UI layer (procedural per-frame).
- `static_assert` on every binary record's `sizeof` catches a whole class of on-disk corruption bugs at compile time.
- Vendoring everything (toolchain, library source, fonts, CMake) trades a small one-time setup cost for deterministic builds on any Windows machine, indefinitely.
- Crash safety does not require a database — two lines of C++ (write `*.tmp`, then `std::rename`) give the same guarantee for whole-file rewrites.

---

## 15.  Limitations & Future Work

**Honest scope. The project is complete against its own objectives. Limitations worth noting:**

- **Single-machine only.** Multi-station LAN sync (front-of-house + kitchen on separate machines) is out of scope.
- **Hash is demo-grade.** `PasswordHash` uses a salted FNV-1a 64-bit mix — clearly labelled as not for production. A real deployment would use a memory-hard KDF (Argon2id, scrypt) via a vetted library.
- **PDF export is text-formatted.** The "export PDF" button writes a monospace `.pdf.txt` file with the receipt content, not a true PDF. True PDF generation needs another library (libharu, etc.).
- **No real printer driver.** Receipts are saved to `.txt`. ESC/POS over USB is straightforward but deployment-specific.
- **No CI test suite.** Relies on preflight, manual exercise, and fresh-machine simulation. A GoogleTest suite for the domain layer is a clean future addition.

**Concrete forward path:** real PDF export · ESC/POS thermal printer hookup · format-version header on binary records · backup/restore UI action · predictive low-stock alerts via rolling averages · multi-station file-watcher mode · i18n via `data/i18n/<lang>.txt` · accessibility pass (keyboard-only nav, larger-text mode, color-blind palette).

---

## 16.  Conclusion

Smart Restaurant POS is a working, self-contained, file-backed point-of-sale desktop application written in C++17. It honors the two constraints that shaped it from day one: it looks and behaves like a real product, and it demonstrates every OOP concept the course requires, mapped to specific named classes in the source.

The codebase is split cleanly into a domain layer (89 files, no UI dependency), an immediate-mode UI layer (28 files, no business state), a tiny platform layer (9 files, the only place that knows about both ImGui and SFML), and a util layer. Persistence is exclusively file-based and crash-safe. The runnable executable plus its DLLs, assets, and seeded data weighs in at about 14 MB, runs offline on any 64-bit Windows 10 or 11 machine, and requires no installer.

Full source, planning, presentation, runnable bundle, and this report:
**[github.com/jazibali2876-jpg/oop-project](https://github.com/jazibali2876-jpg/oop-project)**

---

## 17.  References

1. ISO/IEC 14882:2017. *Information technology — Programming languages — C++.* International Organization for Standardization, 2017.
2. Cornut, O. *Dear ImGui — Bloat-free Graphical User interface for C++ with minimal dependencies.* [github.com/ocornut/imgui](https://github.com/ocornut/imgui)
3. SFML Team. *Simple and Fast Multimedia Library, version 3.1.0.* [www.sfml-dev.org](https://www.sfml-dev.org/)
4. Kitware Inc. *CMake — Cross-platform build system.* [cmake.org](https://cmake.org/)
5. MinGW-w64 Project. *MinGW-w64 — for 32 and 64 bit Windows.* [www.mingw-w64.org](https://www.mingw-w64.org/)
6. Andersson, R. *Inter typeface family.* [rsms.me/inter](https://rsms.me/inter/) (SIL Open Font License 1.1).
7. Font Awesome, Inc. *Font Awesome 6 Free.* [fontawesome.com](https://fontawesome.com/) (CC BY 4.0 for icons, SIL OFL 1.1 for fonts).
8. Stroustrup, B. *The C++ Programming Language, 4th Edition.* Addison-Wesley, 2013.
9. Lippman, S., Lajoie, J., Moo, B. *C++ Primer, 5th Edition.* Addison-Wesley, 2012.
10. Meyers, S. *Effective Modern C++.* O'Reilly Media, 2014.

---

## Appendix A — Repository Layout

```
oop-project/
├── README.md                        Top-level project description.
├── CMakeLists.txt                   Build description.
├── .gitignore                       Excludes build-time bulk + runtime state.
├── planning/                        Ten design documents (01..10).
├── src/                             ALL hand-written C++17 source.
│   ├── main.cpp                     Entry point.
│   ├── App.h / App.cpp              Lifecycle, services bundle, main loop.
│   ├── platform/                    ImGui ↔ SFML 3 integration glue.
│   │   ├── Platform.{h,cpp}         sf::RenderWindow + GL context owner.
│   │   ├── ImGuiBackend.{h,cpp}     ImGui context + OpenGL3 backend.
│   │   ├── InputBridge.{h,cpp}      sf::Event -> ImGuiIO translator.
│   │   └── KeyMap.{h,cpp}           sf::Keyboard::Key -> ImGuiKey table.
│   ├── domain/                      OOP-graded layer. No ImGui/SFML headers.
│   │   ├── common/                  Money, DateTime, Id<Tag>, Capability,
│   │   │                            Exceptions, Result.
│   │   ├── auth/                    User + Admin/Cashier/Kitchen, Session,
│   │   │                            PasswordHash, AuthService,
│   │   │                            UserRepository.
│   │   ├── menu/                    MenuItem, Category, MenuService,
│   │   │                            MenuRepository.
│   │   ├── inventory/               Ingredient, Recipe, InventoryService,
│   │   │                            InventoryRepository, RecipeRepository.
│   │   ├── order/                   OrderItem, Order, OrderStatus,
│   │   │                            OrderService, OrderRepository.
│   │   ├── billing/                 Receipt, PaymentMethod, BillingService,
│   │   │                            ReceiptRepository, PdfExporter.
│   │   ├── customer/                Customer, LoyaltyRule, CustomerService,
│   │   │                            CustomerRepository, LoyaltyRepository.
│   │   ├── tables/                  Table, TableStatus, TableService,
│   │   │                            TableRepository.
│   │   ├── kitchen/                 KitchenTicket, KitchenService.
│   │   ├── analytics/               SalesRecord, Report, AnalyticsService,
│   │   │                            SalesHistoryRepository.
│   │   └── persistence/             FilePaths, TextRepository,
│   │                                BinaryRepository<T> (template),
│   │                                SettingsRepository.
│   ├── ui/                          Immediate-mode UI. No business state.
│   │   ├── AppContext.h             Bundle of service references.
│   │   ├── Router.{h,cpp}           Active-screen state.
│   │   ├── Shell.{h,cpp}            Sidebar + top bar + screen dispatch.
│   │   ├── screens/                 10 immediate-mode draw functions.
│   │   ├── widgets/                 Card, Chart, Sidebar, TopBar, Toast.
│   │   ├── theme/                   Theme, Fonts, Icons.
│   │   ├── anim/                    Animation (easing) + Tween.
│   │   └── state/                   OrderDraft (UI cart), ToastQueue.
│   └── util/                        Strings, Files (writeAtomic), Log, Random.
├── assets/                          Vendored fonts + image folders.
├── data/                            Runtime state (autocreated on first run).
├── dist/                            Self-contained runnable bundle.
│   ├── smart_pos.exe                The application.
│   ├── sfml-*-3.dll                 SFML 3 runtime DLLs.
│   ├── libgcc_s_seh-1.dll, ...      MinGW C++ runtime DLLs.
│   ├── assets/                      Fonts copied next to the exe.
│   ├── data/                        Auto-seeded on first run.
│   └── HOW_TO_RUN.txt               One-paragraph instructions.
├── docs/                            This report.
├── presentation/                    Demo deck (PowerPoint) + generator.
└── tools/                           Preflight script + small C++ probes.
```

---

## Appendix B — Binary File Schemas

Every record uses `#pragma pack(push, 1)` and `static_assert(sizeof(R) == N)`.

### B.1  UserRecord (128 B)
| Field | Type | Bytes | Notes |
|---|---|---|---|
| `id` | `uint32_t` | 4 | Monotonic. |
| `username` | `char[24]` | 24 | NUL-padded. |
| `passwordHash` | `char[32]` | 32 | Exactly 32 hex chars (two FNV-1a runs). |
| `salt` | `char[16]` | 16 | 8 ASCII chars + NUL pad. |
| `fullName` | `char[32]` | 32 | Human-friendly name. |
| `role` | `uint8_t` | 1 | 0=Admin, 1=Cashier, 2=Kitchen. |
| `active` | `uint8_t` | 1 | 1 = enabled. |
| `createdAtEpoch` | `int64_t` | 8 | UNIX seconds. |
| `pad` | `uint8_t[10]` | 10 | Reserved. |

### B.2  OrderHeaderRecord (128 B) + OrderItemRecord (56 B)

OrderHeaderRecord: `id` u32, `placedAtEpoch` i64, `itemCount` u16, `status` u8, `paymentMethod` u8, `discountCents` i64, `taxBp` u16, `customerId` u32, `cashierUsername` char[24], `specialInstructions` char[64], `totalCents` i64, `pad` u8[2].

OrderItemRecord: `menuItemId` u32, `qty` u16, `unitPriceCents` i64, `notes` char[40], `pad` u8[2].

### B.3  CustomerRecord (96 B)

`id` u32, `name` char[40], `phone` char[20], `joinedAtEpoch` i64, `totalSpentCents` i64, `loyaltyPoints` i32, `orderCount` i32, `active` u8, `pad` u8[7].

### B.4  LoyaltyRecord (32 B) — append-only ledger

`customerId` u32, `orderId` u32, `atEpoch` i64, `pointsDelta` i32, `spendCents` i64, `reason` u8, `pad` u8[3].

### B.5  SalesRecord (64 B)

`orderId` u32, `epoch` i64, `totalCents` i64, `itemCount` i16, `paymentMethod` u8, `hour` u8, `cashier` char[24], `customerId` i32, `pad` u8[12].

---

## Appendix C — How to Run & Build

### C.1  Run the existing release

1. Clone or download the repository.
2. Open the `dist/` folder.
3. Double-click `smart_pos.exe`.
4. Sign in with **`admin` / `admin123`** (or `cashier` / `cashier123`, or `kitchen` / `kitchen123`).

Requirements: 64-bit Windows 10 or 11 + a GPU with OpenGL 3.3+. No installer, no admin rights, no network.

### C.2  Build from source

Supply the following alongside the repo (gitignored to keep the repo small):

| Path | What |
|---|---|
| `./SFML-3.1.0/`     | SFML 3.1.0 distribution. |
| `./imgui-master/`   | Dear ImGui master branch. |
| `./mingw64/`        | MinGW-w64 GCC 14.x (UCRT / POSIX / SEH). |
| `./third_party/cmake-*` | CMake portable build. |
| `./third_party/SFML-mingw/` | SFML rebuilt from source with MinGW. |

```bash
export PATH="$PWD/mingw64/bin:$PWD/third_party/SFML-mingw/bin:$PATH"
CMAKE="$PWD/third_party/cmake-3.30.5-windows-x86_64/bin/cmake.exe"

# one-time configure:
"$CMAKE" -S . -B build/project -G "MinGW Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_CXX_COMPILER="$PWD/mingw64/bin/g++.exe" \
    -DCMAKE_MAKE_PROGRAM="$PWD/mingw64/bin/mingw32-make.exe"

# build:
"$CMAKE" --build build/project -- -j 4

# verify:
powershell -ExecutionPolicy Bypass -File ./tools/preflight_check.ps1
```

The output `smart_pos.exe` plus all runtime DLLs plus the `assets/` folder lands in `./dist/`, ready to run.

---

*End of report.*
