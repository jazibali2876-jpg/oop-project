# 04 — OOP Concept → Class/File Map (viva defense)

> Phase 1 planning document. This is the cheat-sheet you defend in viva.
> Every required OOP concept maps to a **primary** demonstration site
> (the one the examiner is shown first) and one or two **supporting** sites.

---

## 1. Master table

| # | OOP concept | Primary site (class — file) | Supporting sites | What to point at |
|---|---|---|---|---|
| 1 | **Classes & objects** | `MenuItem` — `src/domain/menu/MenuItem.h` | `Order`, `Customer`, `Table`, `Ingredient` | Plain data classes with constructors, getters, behavior. |
| 2 | **Encapsulation** | `Order` — `src/domain/order/Order.h` | `Money`, `Customer`, `User` | `Order` keeps `items`, `discount`, `taxRate`, `status` private; mutation is only via `add`, `removeAt`, `setDiscount`, `setStatus`. |
| 3 | **Inheritance** | `User → Admin / Cashier / Kitchen` — `src/domain/auth/` | `TextRepository → MenuRepository`, `BinaryRepository<T> → UserRepository`, etc. | Three-class role hierarchy with a clean abstract base. |
| 4 | **Polymorphism** | `Session::user()` calling `user.can(Capability)` — `src/domain/auth/User.h` + `Session.cpp` | `Receipt` `operator<<` via base `std::ostream&`; repositories called through their base in tests | `Shell` calls `session.user().can(Capability::EditMenu)` and the *runtime type* (Admin/Cashier/Kitchen) decides. |
| 5 | **Abstraction** | `User` — pure virtual `can()` / `roleName()` | `DomainException` base; `Repository` bases | `User` cannot be instantiated; only Admin/Cashier/Kitchen can. |
| 6 | **Virtual functions** | `User::can(Capability) const = 0` + `Admin::can override` | `User::roleName()`, virtual destructor on `User`, `DomainException::what()` | Mark `override` on every subclass — graders look for it. |
| 7 | **Templates** | `BinaryRepository<T>` — `src/domain/persistence/BinaryRepository.h` | `Id<Tag>`, `Result<T>` in `src/domain/common/` | One templated repository instantiated for `UserRecord`, `OrderRecord`, `CustomerRecord`, `LoyaltyRecord`, `SalesRecord`. |
| 8 | **Operator overloading** | `Money` — `+`, `-`, `*`, `==`, `<`, `<<` in `src/domain/common/Money.h` | `Receipt::operator<<` (thermal layout); `Order::operator+=` (`OrderItem`); `Id<T>::operator==/<` | Five+ operators on `Money`, plus stream insertion on `Receipt`. |
| 9 | **Exception handling** | `OrderService::place` catches `InsufficientStockException`, rethrows / lets bubble | UI layer (`OrderScreen.cpp`) catches `DomainException&` → `Toast`; `BinaryRepository::all` throws `CorruptDataException` | Stack trace path: file I/O → repo → service → UI catch → toast. |
| 10 | **STL containers** | `MenuService::items` — `std::vector<MenuItem>`; `Report::itemCounts` — `std::map<MenuItemId,int>`; `TextureCache::textures` — `std::unordered_map<std::string, unique_ptr<sf::Texture>>` | `Report::hourBuckets` — `std::array<int,24>`; `ToastQueue::queue` — `std::deque<Toast>`; `Session::currentUser` — `std::unique_ptr<User>` | At least 6 distinct containers across the project. |
| 11 | **Modular programming** | `src/domain/` split into 10 sub-packages (`auth`, `menu`, `order`, `billing`, `inventory`, `customer`, `tables`, `kitchen`, `analytics`, `persistence`) | Clear domain/UI/platform/util layer separation | Cite the folder tree from `02-folder-structure.md`. |
| 12 | **Header / source separation** | Every domain class — `.h` declares, `.cpp` defines | Pervasive across `src/` | Show two files side-by-side, e.g. `Order.h` and `Order.cpp`. |

---

## 2. Extra credit demonstrations (volunteer in viva if asked)

| Concept | Where | Note |
|---|---|---|
| RAII | `Platform`, `ImGuiBackend`, `Session` | Constructors acquire, destructors release. |
| Smart pointers | `Session::currentUser` (`unique_ptr<User>`), `TextureCache` | `unique_ptr` for ownership, raw refs for borrows. |
| Const-correctness | `MenuService::all() const`, `Order::total() const` everywhere | Const-views on read paths. |
| Rule of zero / five | `BinaryRepository<T>` — explicitly defaults | Document the choice in the header comment. |
| `enum class` (scoped enums) | `Category`, `OrderStatus`, `Capability`, `PaymentMethod`, `TableStatus` | Modern C++ — graders like this. |
| Lambdas | `std::find_if` calls in services, `MenuService::filter` | Used wherever an algorithm is. |
| Move semantics | `AuthService::login` returns `unique_ptr<User>` by value | NRVO + move. |
| `std::optional` | `Order::customer`, `Table::reservedFor`, `OrderService::byId` | Avoids sentinel ids. |

---

## 3. The "viva story" for each concept (one sentence)

These are rehearsed one-liners. Use the exact wording.

1. **Classes & objects** — "Every persisted thing in the system has its own class —
   `MenuItem`, `Order`, `Customer`, `Table` — each with its own state and behavior."
2. **Encapsulation** — "`Order` doesn't expose its items vector; you can only add and
   remove through methods that keep the totals and the inventory in sync."
3. **Inheritance** — "Role behavior is captured in an inheritance hierarchy:
   `Admin`, `Cashier`, and `Kitchen` all derive from an abstract `User` base."
4. **Polymorphism** — "The sidebar asks the *currently logged in* user object what it
   can do; the same call site behaves differently depending on the dynamic type."
5. **Abstraction** — "`User` is pure virtual; the rest of the system never knows or
   cares which concrete role it's holding."
6. **Virtual functions** — "`can(Capability)` is a pure virtual in `User` and each
   subclass overrides it with the matching capability set."
7. **Templates** — "Persistence is a single templated `BinaryRepository<T>` — one
   piece of code stores Users, Orders, Customers, Loyalty entries, and Sales records."
8. **Operator overloading** — "`Money` is a value type with the usual arithmetic and
   comparison operators, and the receipt prints itself via a stream-insertion overload."
9. **Exception handling** — "Domain errors are exception types; services throw them,
   the UI layer catches them and turns them into toasts — there's no error code soup."
10. **STL containers** — "`vector` for live lists, `map` for sorted aggregates,
    `unordered_map` for caches, `array` for fixed-size buckets, `deque` for toasts."
11. **Modular programming** — "The codebase is split into a domain layer, a UI layer,
    a platform/integration layer, and a utility layer — each with its own folder."
12. **Header/source separation** — "Every class lives in a `.h` for declarations and
    a `.cpp` for definitions; templates stay in the header by necessity."

---

## 4. Defense flow (suggested viva order)

If you're given five minutes, present in this order:

1. Open `src/domain/auth/User.h` → show pure virtual `can` (abstraction, virtual).
2. Open `Admin.h` next to it → show `override` (inheritance, polymorphism).
3. Open `Session.cpp` → show `currentUser->can(...)` (polymorphism in action).
4. Open `Money.h` → operator overloading.
5. Open `Order.h` → encapsulation + `operator+=`.
6. Open `BinaryRepository.h` → templates.
7. Open one service file (e.g. `OrderService.cpp`) → exception handling + STL.
8. Cite the folder tree → modular programming + header/source separation.

---

*End of `04-oop-map.md`.*
