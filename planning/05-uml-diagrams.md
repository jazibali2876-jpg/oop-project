# 05 — UML Class Diagrams (Mermaid)

> Phase 1 planning document. All diagrams are in Mermaid `classDiagram` syntax so they
> render directly in GitHub / VS Code / Obsidian without any extra tooling.
> Several smaller diagrams are easier to read than one mega-diagram, so this file
> splits by domain package and finishes with an overview.

---

## 1. Auth package

```mermaid
classDiagram
    direction TB
    class User {
        <<abstract>>
        -string username
        -string passwordHash
        -string fullName
        -DateTime createdAt
        +can(Capability) bool*
        +roleName() string*
        +displayName() string
        +~User()
    }

    class Admin {
        +can(Capability) bool
        +roleName() string
    }

    class Cashier {
        +can(Capability) bool
        +roleName() string
    }

    class Kitchen {
        +can(Capability) bool
        +roleName() string
    }

    class Session {
        -unique_ptr~User~ currentUser
        -DateTime loginAt
        +isAuthenticated() bool
        +user() User&
        +clear()
    }

    class AuthService {
        -UserRepository& repo
        +login(user, pwd) unique_ptr~User~
        +logout(Session&)
        +addUser(unique_ptr~User~)
        +rememberMe(username)
    }

    User <|-- Admin
    User <|-- Cashier
    User <|-- Kitchen
    Session o-- User : owns unique_ptr
    AuthService ..> User : creates
    AuthService --> UserRepository : uses
```

---

## 2. Menu + Inventory packages

```mermaid
classDiagram
    direction TB
    class MenuItem {
        -MenuItemId id
        -string name
        -Category category
        -Money price
        -string imagePath
        -bool available
        +matches(query) bool
    }

    class MenuService {
        -MenuRepository& repo
        -vector~MenuItem~ items
        +all() vector~MenuItem~&
        +filter(Category, query) vector~MenuItem~
        +byId(MenuItemId) MenuItem&
        +add(MenuItem)
        +update(MenuItem)
        +remove(MenuItemId)
        +persist()
    }

    class Ingredient {
        -IngredientId id
        -string name
        -string unit
        -double stock
        -double reorderThreshold
        -Money costPerUnit
        +isLow() bool
    }

    class Recipe {
        -MenuItemId menuItem
        -vector~pair~ components
    }

    class InventoryService {
        -InventoryRepository& invRepo
        -RecipeRepository& recRepo
        +canFulfill(Order) bool
        +deduct(Order)
        +lowStock() vector~Ingredient~
        +usageBetween(from, to) map
    }

    MenuService o-- MenuItem
    MenuService --> MenuRepository : uses
    InventoryService o-- Ingredient
    InventoryService o-- Recipe
    InventoryService --> InventoryRepository : uses
    InventoryService --> RecipeRepository : uses
    Recipe --> MenuItem : refs by id
```

---

## 3. Order + Billing packages

```mermaid
classDiagram
    direction TB
    class OrderItem {
        -MenuItemId menuItem
        -int qty
        -Money unitPrice
        -string notes
        +lineTotal() Money
    }

    class Order {
        -OrderId id
        -DateTime placedAt
        -vector~OrderItem~ items
        -Money discount
        -double taxRate
        -OrderStatus status
        -optional~CustomerId~ customer
        -string cashierUsername
        -string specialInstructions
        +add(OrderItem)
        +removeAt(idx)
        +setDiscount(Money)
        +subtotal() Money
        +tax() Money
        +total() Money
        +estimatedPrepMinutes() int
        +setStatus(OrderStatus)
        +empty() bool
        +operator+=(OrderItem) Order&
    }

    class OrderService {
        -OrderRepository& orders
        -InventoryService& inv
        -KitchenService& kit
        -SalesHistoryRepository& sales
        +place(draft, by, customer) Order
        +cancel(id, by)
        +setStatus(id, status, by)
        +active() vector~Order~
        +byId(id) optional~Order~
    }

    class Receipt {
        -OrderId orderId
        -string customerName
        -vector~OrderItem~ items
        -Money subtotal
        -Money discount
        -Money tax
        -Money total
        -PaymentMethod method
        -DateTime issuedAt
        +operator<<(ostream, Receipt) friend
    }

    class BillingService {
        -ReceiptRepository& receipts
        -CustomerService& customers
        +makeReceipt(Order) Receipt
        +recordPayment(Order&, method)
        +saveReceiptFile(Receipt) string
        +exportPdf(Receipt, path)
    }

    Order o-- OrderItem
    OrderService --> Order : creates
    OrderService --> InventoryService
    OrderService --> KitchenService
    Receipt o-- OrderItem
    BillingService --> Receipt : creates
    BillingService --> ReceiptRepository : uses
    BillingService --> CustomerService
```

---

## 4. Customer + Tables + Kitchen + Analytics

```mermaid
classDiagram
    direction TB
    class Customer {
        -CustomerId id
        -string name
        -string phone
        -DateTime joinedAt
        -Money totalSpent
        -int loyaltyPoints
        -int orderCount
        +recordOrder(Money)
        +tier() string
    }

    class LoyaltyRule {
        -int pointsPerDollar
        -map tierThresholds
        +pointsFor(Money) int
        +tierFor(points) string
    }

    class CustomerService {
        -CustomerRepository& repo
        -LoyaltyRule rule
        +findByPhoneOrCreate(phone) Customer
        +findById(id) optional~Customer~
        +search(q) vector~Customer~
        +applyLoyalty(id, total)
        +history(id, OrderRepository&) vector~Order~
    }

    class Table {
        -TableId id
        -int seats
        -TableStatus status
        -optional~CustomerId~ reservedFor
        -DateTime reservedAt
    }

    class TableService {
        +all() vector~Table~
        +reserve(id, customer)
        +occupy(id)
        +free(id)
        +persist()
    }

    class KitchenTicket {
        -OrderId orderId
        -DateTime queuedAt
        -OrderStatus status
        -vector~OrderItem~ items
    }

    class KitchenService {
        -vector~KitchenTicket~ tickets
        +enqueue(Order)
        +advance(orderId)
        +lane(status) vector~KitchenTicket~
    }

    class SalesRecord {
        +uint32_t orderId
        +int64_t epoch
        +int64_t totalCents
        +int8_t paymentMethod
        +int16_t itemCount
        +char cashier[24]
    }

    class Report {
        -DateTime from
        -DateTime to
        -Money revenue
        -int orderCount
        -map itemCounts
        -array hourBuckets
        +topItems(n) vector
        +peakHour() int
    }

    class AnalyticsService {
        -SalesHistoryRepository& sales
        -MenuRepository& menu
        +buildReport(from, to) Report
        +today() Report
        +thisWeek() Report
        +thisMonth() Report
        +employeePerformance(from, to) map
    }

    CustomerService o-- Customer
    CustomerService --> LoyaltyRule
    TableService o-- Table
    KitchenService o-- KitchenTicket
    AnalyticsService --> SalesRecord : reads
    AnalyticsService --> Report : builds
```

---

## 5. Persistence (the template demo)

```mermaid
classDiagram
    direction TB
    class TextRepository {
        <<base>>
        #readAllLines(path) vector~string~
        #writeAllLines(path, lines)
    }

    class BinaryRepository~T~ {
        <<template base>>
        -string path
        +all() vector~T~
        +append(T)
        +overwriteAll(vector~T~)
        +findIf(Pred) optional~T~
    }

    class UserRepository {
        +all() vector~UserRecord~
        +findByUsername(name) optional~UserRecord~
        +upsert(UserRecord)
    }

    class MenuRepository {
        +all() vector~MenuItem~
        +save(vector~MenuItem~)
    }

    class OrderRepository {
        +append(Order)
        +all() vector~Order~
        +byId(id) optional~Order~
    }

    class ReceiptRepository {
        +write(Receipt) string
        +pathFor(orderId) string
    }

    class CustomerRepository
    class InventoryRepository
    class RecipeRepository
    class TableRepository
    class LoyaltyRepository
    class SalesHistoryRepository
    class SettingsRepository

    TextRepository <|-- MenuRepository
    TextRepository <|-- InventoryRepository
    TextRepository <|-- RecipeRepository
    TextRepository <|-- TableRepository
    TextRepository <|-- SettingsRepository
    TextRepository <|-- ReceiptRepository

    BinaryRepository <|-- UserRepository
    BinaryRepository <|-- OrderRepository
    BinaryRepository <|-- CustomerRepository
    BinaryRepository <|-- LoyaltyRepository
    BinaryRepository <|-- SalesHistoryRepository
```

---

## 6. Platform / integration layer

```mermaid
classDiagram
    direction TB
    class App {
        -Platform platform
        -ImGuiBackend imgui
        -Services services
        -Session session
        -Router router
        -Theme theme
        -TextureCache textures
        -AudioPlayer audio
        -ToastQueue toasts
        +run() int
        +tick(dt)
        +shutdown()
    }

    class Platform {
        -unique_ptr~RenderWindow~ window
        -sf::Clock clock
        +init(Config) bool
        +pumpEvents(InputBridge&) bool
        +beginFrame()
        +endFrame()
    }

    class ImGuiBackend {
        +init(window) bool
        +newFrame()
        +render(window)
        +shutdown()
    }

    class InputBridge {
        -KeyMap keymap
        +handle(sf::Event)
        +newFrame(dt, size)
    }

    class KeyMap {
        +translate(sf::Keyboard::Key) ImGuiKey
    }

    class TextureCache {
        -map textures
        +get(path) ImTextureID
        +clear()
    }

    class AudioPlayer {
        -map buffers
        -vector activeSounds
        +play(name)
        +preload(path)
    }

    App o-- Platform
    App o-- ImGuiBackend
    App o-- TextureCache
    App o-- AudioPlayer
    Platform --> InputBridge : forwards events
    InputBridge o-- KeyMap
```

---

## 7. UI layer skeleton

```mermaid
classDiagram
    direction TB
    class Shell {
        +draw(AppContext&)
    }

    class Router {
        -Screen current
        -vector~Screen~ history
        +go(Screen)
        +back()
    }

    class AppContext {
        +AuthService&
        +MenuService&
        +OrderService&
        +BillingService&
        +InventoryService&
        +CustomerService&
        +TableService&
        +KitchenService&
        +AnalyticsService&
        +Session&
        +Router&
        +Theme&
        +TextureCache&
        +AudioPlayer&
        +ToastQueue&
    }

    class Theme {
        +apply(Mode)
    }

    class ToastQueue {
        -deque~Toast~ queue
        +push(text, kind)
        +draw()
        +tick(dt)
    }

    Shell ..> AppContext : reads
    Shell ..> Router : reads/writes
    AppContext ..> Theme
    AppContext ..> ToastQueue
```

UI screens are intentionally *not* classes — they are free functions in
`ui::screens::Draw*`. They appear in `07-data-flow.md` and `09-gui-wireframes.md`
instead of in the class diagram.

---

## 8. End-to-end overview (services + ownership)

```mermaid
classDiagram
    direction LR
    class App
    class Platform
    class ImGuiBackend
    class Services {
        +AuthService
        +MenuService
        +OrderService
        +BillingService
        +InventoryService
        +CustomerService
        +TableService
        +KitchenService
        +AnalyticsService
    }
    class Repositories {
        +UserRepository
        +MenuRepository
        +OrderRepository
        +ReceiptRepository
        +CustomerRepository
        +InventoryRepository
        +RecipeRepository
        +TableRepository
        +LoyaltyRepository
        +SalesHistoryRepository
        +SettingsRepository
    }
    class Files {
        users.dat
        menu.txt
        orders.dat
        receipts/RCPT-*.txt
        customers.dat
        inventory.txt
        recipes.txt
        tables.txt
        loyalty.dat
        sales_history.dat
        settings.txt
    }

    App o-- Platform
    App o-- ImGuiBackend
    App o-- Services
    App o-- Repositories
    Services --> Repositories : uses
    Repositories --> Files : reads/writes
```

---

*End of `05-uml-diagrams.md`.*
