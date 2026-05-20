# 03 — Class Design (Smart Restaurant POS)

> Phase 1 planning document. No code is written.
> Every entry below specifies **responsibility**, **key members**, **key methods**,
> **relationships**, and **OOP markers** (which OOP concept this class is the
> primary demonstration site for — full mapping in `04-oop-map.md`).
> Field types use C++ syntax for clarity but no `.h`/`.cpp` is produced this phase.

---

## A. Integration-layer classes — `src/platform/`

These classes are the only place that includes BOTH SFML 3 and ImGui headers.

### A1. `Platform`
- **Responsibility:** create the `sf::RenderWindow` with an OpenGL context, manage window
  lifecycle, expose framebuffer size + a clock.
- **Members:**
  ```
  std::unique_ptr<sf::RenderWindow> window;
  sf::Clock clock;
  sf::Vector2u size;
  ```
- **Methods:**
  - `bool init(const Config&)` — create window, `setActive(true)`, `setVerticalSyncEnabled(true)`.
  - `bool pumpEvents(InputBridge&)` — drain `pollEvent()` (SFML 3 returns `std::optional<sf::Event>`),
    forwards each event to `InputBridge`, returns `false` if a `Closed` event arrived.
  - `void beginFrame() / endFrame()` — clear, present.
  - `sf::Vector2u framebufferSize() const`, `float deltaSeconds()`.
- **Relationships:** composes `InputBridge`, used by `App`.

### A2. `ImGuiBackend`
- **Responsibility:** own the ImGui context and the OpenGL3 backend lifecycle.
- **Methods:**
  - `bool init(sf::RenderWindow&)` — `ImGui::CreateContext`, configure `ImGuiIO`,
    load fonts via `Fonts::load`, call `ImGui_ImplOpenGL3_Init`.
  - `void newFrame()` — `ImGui_ImplOpenGL3_NewFrame` + `ImGui::NewFrame`.
  - `void render(sf::RenderWindow&)` — `ImGui::Render` + `ImGui_ImplOpenGL3_RenderDrawData`.
  - `void shutdown()` — backend shutdown + `ImGui::DestroyContext`.
- **Relationships:** used by `App` once. Holds no per-frame state itself.

### A3. `InputBridge`
- **Responsibility:** translate SFML 3 events into ImGui input events; maintain
  per-frame `io.DeltaTime`, `io.DisplaySize`.
- **Members:**
  ```
  KeyMap keymap;
  float  lastTime = 0.f;
  ```
- **Methods:**
  - `void handle(const sf::Event&)` — branch on `event.is<sf::Event::Closed>()`,
    `event.getIf<sf::Event::MouseMoved>()`, `MouseButtonPressed/Released`,
    `MouseWheelScrolled`, `KeyPressed/KeyReleased`, `TextEntered`, `Resized`,
    `FocusGained/FocusLost`, and call the matching `ImGuiIO::Add*Event`.
  - `void newFrame(float dt, sf::Vector2u size)` — set `io.DeltaTime`,
    `io.DisplaySize`.
- **Relationships:** owns `KeyMap`.

### A4. `KeyMap`
- **Responsibility:** static lookup of `sf::Keyboard::Key` → `ImGuiKey`. Uses scoped
  enums (SFML 3 API).
- **Methods:** `ImGuiKey translate(sf::Keyboard::Key k) const`.

### A5. `TextureCache`
- **Responsibility:** load image files into `sf::Texture` once, hand back an
  `ImTextureID` for `ImGui::Image`.
- **Members:** `std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;`
- **Methods:**
  - `ImTextureID get(const std::string& path)` — load if missing, return
    `(ImTextureID)(intptr_t)tex.getNativeHandle()`.
  - `void clear()`.
- **Relationships:** used by every UI screen that draws an image.

### A6. `AudioPlayer`
- **Responsibility:** load + play short sound effects.
- **Members:** `std::unordered_map<std::string, sf::SoundBuffer> buffers;`
  `std::vector<sf::Sound> activeSounds;` (small ring for short SFX).
- **Methods:** `void play(const std::string& name)`, `void preload(const std::string& path)`.

### A7. `Clock`
- **Responsibility:** frame delta + wall-clock helpers wrapping `sf::Clock` and `std::chrono`.

---

## B. Domain `common` — `src/domain/common/`

### B1. `Money` (value class) — **OOP: operator overloading, encapsulation**
- **Members:** `int64_t cents;` (private)
- **Constructors:** `Money() = default; explicit Money(int64_t cents); static Money fromDouble(double);`
- **Operators:** `+, -, *(int), ==, !=, <, <=, >, >=`, `operator<<(std::ostream&, Money)`
  formatted as `$12.34`.
- **Methods:** `double toDouble() const;` `Money withTax(double rate) const;`

### B2. `DateTime` (value class)
- **Members:** `std::time_t epoch;`
- **Methods:** `static DateTime now();` `std::string format(const char*) const;`
  `int dayOfYear() const;` `int hour() const;`
- **Operators:** `==, <, <=, >, >=` for trend/window logic.

### B3. `Id<Tag>` (templated strong typedef) — **OOP: templates**
- **Form:** `template<class Tag> class Id { uint32_t v; ... };`
- **Specializations as aliases:** `using OrderId = Id<struct OrderIdTag>;`
  `using CustomerId = Id<...>;` etc.
- **Methods:** `uint32_t value() const;` `static Id next(uint32_t lastSeen);`
- **Operators:** `==, !=, <` (for std::map keys), `operator<<`.

### B4. `Capability` (enum class)
- Values: `ViewMenu, EditMenu, PlaceOrder, EditInventory, ViewKitchen, AdvanceTicket,
  ManageTables, ManageCustomers, ViewAnalytics, ManageUsers`.

### B5. `DomainException` and friends — **OOP: exception handling**
- `class DomainException : public std::runtime_error { ... };`
- Subclasses (each adds context fields):
  - `InvalidCredentialsException`
  - `EmptyOrderException`
  - `InsufficientStockException` (with `ingredientId`, `needed`, `have`)
  - `DuplicateIdException`
  - `NotFoundException`
  - `FileIOException` (with `path`, `op`)
  - `CorruptDataException`
  - `UnauthorizedException` (with `Capability`)
- Caught at the UI boundary, mapped to toasts.

### B6. `Result<T>` (template) — **OOP: templates**
- A tiny `std::variant`-backed `Result<T>` used in spots where exceptions would be
  too heavy (e.g. UI quick-check on a draft). `bool ok() const; T& value(); std::string error();`

---

## C. Auth — `src/domain/auth/`

### C1. `User` (abstract) — **OOP: abstraction, virtual functions**
- **Members:** `std::string username; std::string passwordHash; std::string fullName; DateTime createdAt;`
- **Pure virtual:** `virtual bool can(Capability) const = 0;`
- **Virtual:** `virtual std::string roleName() const = 0;`
- **Destructor:** `virtual ~User() = default;`
- **Encapsulation:** all fields private with const accessors.

### C2. `Admin : public User` — **OOP: inheritance, polymorphism**
- `can()` returns `true` for all capabilities.

### C3. `Cashier : public User`
- `can()` enables `ViewMenu, PlaceOrder, ManageTables, ManageCustomers`.

### C4. `Kitchen : public User`
- `can()` enables `ViewKitchen, AdvanceTicket` only.

### C5. `Session`
- **Members:** `std::unique_ptr<User> currentUser; DateTime loginAt;`
- **Methods:** `bool isAuthenticated() const; const User& user() const;` `void clear();`

### C6. `PasswordHash`
- **Methods (free or static):** `std::string hash(const std::string& pwd, const std::string& salt);`
  `bool verify(...)`. Simple salted hash — clearly labelled "demo-grade".

### C7. `AuthService`
- **Members:** ref to `UserRepository`.
- **Methods:**
  - `std::unique_ptr<User> login(std::string user, std::string pwd)` — throws `InvalidCredentialsException`.
  - `void logout(Session&);`
  - `void addUser(std::unique_ptr<User>);` (Admin only — enforced by caller)
  - `void rememberMe(const std::string& username);` (writes to `settings.txt`)
- **Relationships:** depends on `UserRepository`. Returns polymorphic `unique_ptr<User>`.

---

## D. Menu — `src/domain/menu/`

### D1. `MenuItem`
- **Members:** `MenuItemId id; std::string name; Category category; Money price; std::string imagePath; bool available;`
- **Methods:** standard getters/setters; `bool matches(const std::string& q) const;`
  case-insensitive substring search on name + category.

### D2. `Category` (enum class): `FastFood, BBQ, Chinese, Drinks, Desserts`.

### D3. `MenuService`
- **Members:** ref to `MenuRepository`; `std::vector<MenuItem> items;` (in-memory cache).
- **Methods:**
  - `const std::vector<MenuItem>& all() const;`
  - `std::vector<MenuItem> filter(Category, const std::string& query) const;`
  - `const MenuItem& byId(MenuItemId) const;` — throws `NotFoundException`.
  - `void add(MenuItem);` — throws `DuplicateIdException`.
  - `void update(const MenuItem&);`
  - `void remove(MenuItemId);`
  - `void persist();` — delegates to repository.
- **STL:** `std::vector`, `std::find_if`, `std::sort`.

---

## E. Inventory — `src/domain/inventory/`

### E1. `Ingredient`
- **Members:** `IngredientId id; std::string name; std::string unit; double stock; double reorderThreshold; Money costPerUnit;`
- **Methods:** `bool isLow() const { return stock <= reorderThreshold; }`

### E2. `Recipe`
- **Members:** `MenuItemId menuItem; std::vector<std::pair<IngredientId, double>> components;`

### E3. `InventoryService`
- **Members:** refs to `InventoryRepository`, `RecipeRepository`.
- **Methods:**
  - `bool canFulfill(const Order&) const;`
  - `void deduct(const Order&);` — throws `InsufficientStockException` if a race condition occurs.
  - `std::vector<Ingredient> lowStock() const;`
  - `std::map<IngredientId, double> usageBetween(DateTime, DateTime) const;`

---

## F. Order — `src/domain/order/`

### F1. `OrderItem`
- **Members:** `MenuItemId menuItem; int qty; Money unitPrice; std::string notes;`
- **Methods:** `Money lineTotal() const { return unitPrice * qty; }`.

### F2. `OrderStatus` (enum class): `Pending, Preparing, Ready, Served, Cancelled`.

### F3. `Order` — **OOP: encapsulation, operator overloading**
- **Members (private):**
  ```
  OrderId id;
  DateTime placedAt;
  std::vector<OrderItem> items;
  Money discount;
  double taxRate;
  OrderStatus status;
  std::optional<CustomerId> customer;
  std::string cashierUsername;
  std::string specialInstructions;
  ```
- **Methods:** `void add(const OrderItem&);` `void removeAt(size_t);` `void setDiscount(Money);`
  `Money subtotal() const; Money tax() const; Money total() const;` `int estimatedPrepMinutes() const;`
  `void setStatus(OrderStatus);` `bool empty() const;`
- **Operators:**
  - `Order& operator+=(const OrderItem&);` (sugar for `add`)
  - `friend std::ostream& operator<<(std::ostream&, const Order&);` (debug printout)

### F4. `OrderService`
- **Members:** refs to `OrderRepository, InventoryService, KitchenService, SalesHistoryRepository`.
- **Methods:**
  - `Order place(Order draft, const User& by, std::optional<CustomerId>);`
    - validates non-empty, checks capability, asks inventory, deducts inventory,
      assigns id + timestamp, persists, enqueues to kitchen, appends sales record.
  - `void cancel(OrderId, const User& by);`
  - `void setStatus(OrderId, OrderStatus, const User& by);`
  - `std::vector<Order> active() const;`
  - `std::optional<Order> byId(OrderId) const;`

---

## G. Billing — `src/domain/billing/`

### G1. `PaymentMethod` (enum class): `Cash, Card, OnlineSim`.

### G2. `Receipt` — **OOP: operator overloading**
- **Members:** `OrderId orderId; std::string customerName; std::vector<OrderItem> items;
  Money subtotal, discount, tax, total; PaymentMethod method; DateTime issuedAt;`
- **Operators:** `friend std::ostream& operator<<(std::ostream&, const Receipt&);` —
  emits the thermal-style 42-column layout (used to write `receipts/RCPT-*.txt`).

### G3. `PdfExporter`
- **Methods:** `void write(const Receipt&, const std::string& path);`
- Writes a text-formatted "PDF-style" file (not a real PDF — clearly labelled). The
  brief allows this (no PDF library is in scope).

### G4. `BillingService`
- **Members:** refs to `ReceiptRepository`, `CustomerService`.
- **Methods:**
  - `Receipt makeReceipt(const Order&) const;`
  - `void recordPayment(Order&, PaymentMethod);`
  - `std::string saveReceiptFile(const Receipt&);` (returns the file path)
  - `void exportPdf(const Receipt&, const std::string& outPath);`

---

## H. Customer — `src/domain/customer/`

### H1. `Customer`
- **Members:** `CustomerId id; std::string name; std::string phone; DateTime joinedAt;
  Money totalSpent; int loyaltyPoints; int orderCount;`
- **Methods:** `void recordOrder(Money total); std::string tier() const;` (Bronze/Silver/Gold)

### H2. `LoyaltyRule`
- **Members:** `int pointsPerDollar; std::map<std::string, int> tierThresholds;`
- **Methods:** `int pointsFor(Money) const; std::string tierFor(int points) const;`

### H3. `CustomerService`
- **Members:** ref to `CustomerRepository`, `LoyaltyRule`.
- **Methods:**
  - `Customer findByPhoneOrCreate(const std::string&);`
  - `std::optional<Customer> findById(CustomerId) const;`
  - `std::vector<Customer> search(const std::string& q) const;`
  - `void applyLoyalty(CustomerId, Money orderTotal);` — bumps points + totalSpent.
  - `std::vector<Order> history(CustomerId, const OrderRepository&) const;`

---

## I. Tables — `src/domain/tables/`

### I1. `TableStatus` (enum class): `Free, Occupied, Reserved`.

### I2. `Table`
- **Members:** `TableId id; int seats; TableStatus status; std::optional<CustomerId> reservedFor; DateTime reservedAt;`

### I3. `TableService`
- **Methods:**
  - `std::vector<Table> all() const;`
  - `void reserve(TableId, CustomerId);`
  - `void occupy(TableId);`
  - `void free(TableId);`
  - `void persist();`

---

## J. Kitchen — `src/domain/kitchen/`

### J1. `KitchenTicket`
- **Members:** `OrderId orderId; DateTime queuedAt; OrderStatus status; std::vector<OrderItem> items;`

### J2. `KitchenService`
- **Members:** `std::vector<KitchenTicket> tickets;` (in-memory, hydrated from active orders on boot).
- **Methods:**
  - `void enqueue(const Order&);`
  - `void advance(OrderId);` — Pending → Preparing → Ready → Served (removes from queue).
  - `std::vector<KitchenTicket> lane(OrderStatus) const;`

---

## K. Analytics — `src/domain/analytics/`

### K1. `SalesRecord` (POD-ish; goes into `BinaryRepository<SalesRecord>`)
- **Fields:** `uint32_t orderId; int64_t epoch; int64_t totalCents; int8_t paymentMethod;
  int16_t itemCount; char cashier[24];`

### K2. `Report`
- **Members:** `DateTime from, to; Money revenue; int orderCount; std::map<MenuItemId, int> itemCounts;
  std::array<int, 24> hourBuckets;`
- **Methods:** `std::vector<std::pair<MenuItemId,int>> topItems(int n) const;` `int peakHour() const;`

### K3. `AnalyticsService`
- **Members:** refs to `SalesHistoryRepository, MenuRepository`.
- **Methods:**
  - `Report buildReport(DateTime from, DateTime to) const;`
  - `Report today() const; Report thisWeek() const; Report thisMonth() const;`
  - `std::map<std::string, int> employeePerformance(DateTime from, DateTime to) const;` (orderCount per cashier)
- **STL:** `std::map, std::array, std::accumulate, std::sort`.

---

## L. Persistence — `src/domain/persistence/`

### L1. `FilePaths`
- All `data/*` paths as `constexpr` string views.

### L2. `TextRepository` (base) — **OOP: inheritance, encapsulation**
- **Methods (protected):**
  - `std::vector<std::string> readAllLines(const std::string& path) const;`
  - `void writeAllLines(const std::string&, const std::vector<std::string>&);`
    (write-temp-then-`std::rename`)
- **Throws:** `FileIOException` on stream failure.

### L3. `BinaryRepository<T>` (template base) — **OOP: templates, inheritance**
- **Constraint (documented, not C++20 concept):** `T` must be trivially copyable.
- **Members:** `std::string path;`
- **Methods:**
  - `std::vector<T> all() const;` — `ifstream(path, std::ios::binary)`, repeated `read`.
  - `void append(const T&);` — `ofstream(path, std::ios::binary | std::ios::app)`.
  - `void overwriteAll(const std::vector<T>&);` — write-temp-then-rename.
  - `std::optional<T> findIf(Pred) const;` (templated member)
- **Why a template:** record type varies per repository — single implementation
  satisfies the "templates" OOP marker cleanly.

### L4–L14. Concrete repositories (each is small)

Each inherits from either `TextRepository` or `BinaryRepository<RecordStruct>` and adds
the named CRUD operations. Listed here only by name; the data layout is in `08-file-schemas.md`.

- `UserRepository`           — binary  (`UserRecord`)
- `MenuRepository`           — text    (`menu.txt`)
- `InventoryRepository`      — text    (`inventory.txt`)
- `RecipeRepository`         — text    (`recipes.txt`)
- `OrderRepository`          — binary  (`OrderHeaderRecord` + appended `OrderItemRecord` lines)
- `ReceiptRepository`        — text    (one file per receipt under `data/receipts/`)
- `CustomerRepository`       — binary  (`CustomerRecord`)
- `LoyaltyRepository`        — binary  (`LoyaltyRecord`)
- `SalesHistoryRepository`   — binary  (`SalesRecord`)
- `TableRepository`          — text    (`tables.txt`)
- `SettingsRepository`       — text    (`settings.txt` k=v)

Each translates raw stream errors into `FileIOException` / `CorruptDataException`.

---

## M. UI layer — `src/ui/`

UI classes are deliberately thin — they exist mostly so the screens have shared shell
code. They hold **no business state**.

### M1. `AppContext`
- **Members:** references to every service + `Session&` + `Router&` + `Theme&` + `TextureCache&` + `AudioPlayer&` + `ToastQueue&`.
- Passed by reference into every screen draw function.

### M2. `Router`
- **Members:** `Screen current; std::vector<Screen> history;`
- **Methods:** `void go(Screen); void back();` (sidebar buttons call `go`).

### M3. `Shell`
- **Responsibility:** draw the persistent chrome (sidebar + top bar) and dispatch
  to the active screen draw function.
- **Methods:** `void draw(AppContext&);`

### M4. `Theme` / `Fonts` / `Icons`
- **Theme:** `void apply(Mode);` rebuilds `ImGui::GetStyle()`.
- **Fonts:** `bool load(ImGuiIO&);` adds regular + bold + icon font from `assets/fonts/`.
- **Icons:** constexpr `const char* ICON_HOME = "";` etc. (Font Awesome codepoints).

### M5. Widgets (no per-widget state classes — these are free functions in `ui::widgets::`)
- `void Card(const char* title, const std::string& value, ImVec4 accent);`
- `bool IconButton(const char* icon, const char* label);`
- `void BarChart(const std::vector<BarDatum>&, ImVec2 size);`
- `void PieChart(const std::vector<PieSlice>&, float radius);`
- `void LineChart(const std::vector<float>&, ImVec2 size);`
- `bool SearchBox(char* buf, size_t len);`
- `void Modal(const char* id, ModalFn body);`

### M6. UI-state structs (in `src/ui/state/`)
- `OrderDraft` — items in the cart being built (NOT yet a domain `Order`).
- `MenuFilter` — `Category cat; std::string query;`
- `AnalyticsFilter` — `DateTime from, to; Grouping grouping;`
- `ToastQueue` — `std::deque<Toast> queue;` with TTL countdown.

### M7. Animations
- `Animation::easeOutQuad(float t)`
- `Tween { float current, target, speed; void step(float dt); }`

---

## N. App & main — `src/`

### N1. `App`
- **Members:** `Platform platform; ImGuiBackend imgui; Services services; Session session; Router router; Theme theme; TextureCache textures; AudioPlayer audio; ToastQueue toasts;`
- **Methods:**
  - `int run();` — main loop (see `01-architecture.md` §3.1).
  - `void tick(float dt);` — domain-side per-frame work (animation timers in UI state,
    autosave debounce, low-stock recheck).
  - `void shutdown();` — flush dirty repos, close window.

### N2. `Services`
- A small POD bundling references to all domain services for easy passing into
  `AppContext`.

---

## O. Relationships at a glance

- `App` owns `Platform`, `ImGuiBackend`, every repository, every service, `Session`, `Router`, `Theme`.
- Each service owns NOTHING — it holds references to repos and other services.
- Each repository owns one file path string.
- `Session` owns `std::unique_ptr<User>` — the only polymorphic ownership in the project.
- UI screens hold zero ownership; they receive `AppContext&` per frame.

---

*End of `03-class-design.md`.*
