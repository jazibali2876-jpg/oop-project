# 01 — Architecture (Smart Restaurant POS)

> Phase 1 planning document. No code. No build.
> Stack is fixed: **C++17 + MinGW-w64 GCC + SFML 3.1.0 + Dear ImGui (manual OpenGL3 integration) + CMake (Phase 2)**.
> Storage: **C++ file handling only** (text + binary). No SQL.

---

## 1. Architectural goals

1. Demonstrate **strong OOP** for a final-year viva: classes, encapsulation, inheritance,
   polymorphism, abstraction, virtual functions, templates, operator overloading,
   exception handling, STL, modular header/source split.
2. Look and feel like commercial POS software (Toast / Square / Lightspeed style) —
   sidebar nav, dashboard cards, dark/light theme, animated buttons, charts.
3. Stay **realistically buildable** by university students inside one semester —
   no enterprise frameworks, no databases, no networking beyond an "online payment"
   simulation that writes to a local file.
4. Survive the **immediate-mode reality** of Dear ImGui without losing the OOP grade:
   ImGui has no persistent widget objects, no `.ui` files, no signals/slots. We will
   not pretend otherwise — we will architect around it.

---

## 2. Two-layer split (the OOP-safe immediate-mode pattern)

The single most important architectural decision in this project:

```
+----------------------------------------------------------+
|                        UI LAYER                          |
|  (per-frame, procedural, ImGui draw calls + SFML window) |
|                                                          |
|  Screens: Splash, Login, Dashboard, Menu, Order, Bill,   |
|           Inventory, Tables, Kitchen, Analytics          |
|  Holds:   transient UI state only                        |
|           (open panel, selected row, animation timers)   |
+--------------------------+-------------------------------+
                           |  reads / calls into
                           v
+----------------------------------------------------------+
|                     DOMAIN LAYER                         |
|     (plain C++17, knows nothing about ImGui/SFML)        |
|                                                          |
|  Services:   AuthService, MenuService, OrderService,     |
|              BillingService, InventoryService,           |
|              CustomerService, TableService,              |
|              KitchenService, AnalyticsService            |
|  Entities:   User, MenuItem, Order, OrderItem, Receipt,  |
|              Customer, Table, Ingredient, ...            |
|  Repos:      *Repository classes — own ALL file I/O      |
|                                                          |
|  Holds: ALL business state, ALL persisted data.          |
|  This is the layer graded for OOP.                       |
+--------------------------+-------------------------------+
                           |  text + binary file I/O
                           v
+----------------------------------------------------------+
|                       DATA FILES                         |
|  data/users.dat (binary)   data/menu.txt                 |
|  data/orders.dat (binary)  data/inventory.txt            |
|  data/customers.dat (bin)  data/receipts/*.txt           |
|  data/tables.txt           data/sales_history.dat (bin)  |
|  data/settings.txt         data/loyalty.dat (bin)        |
+----------------------------------------------------------+
```

**Hard rules of the split**

- The domain layer **must not include any ImGui or SFML header**.
- The UI layer **must not contain business state** beyond what is needed to draw the
  current frame (selection, scroll, hover, animation timer).
- The UI layer **never writes to files directly** — it always goes through a domain
  service, which goes through a repository.
- ImGui's per-frame `if (ImGui::Button(...))` returns drive **calls into domain
  services**, not changes to ImGui-owned state.

**Why this split matters for the grade.** ImGui's procedural style would otherwise
tempt students to put logic inside button handlers, killing the OOP demonstration.
By making the UI layer *thin* and the domain layer *fat*, we get a real OOP project
behind a modern-looking UI.

---

## 3. The manual ImGui ↔ SFML integration layer

There is no ImGui-SFML binding and we are not adding one. The integration is **hand-written**.
It lives in `src/platform/` and is a small set of classes/free-functions whose job is to
glue SFML 3's window/event loop to ImGui's input/render lifecycle, using ImGui's stock
OpenGL3 backend from `imgui-master/backends/`.

### 3.1 What it must do

1. **Own the SFML window with an OpenGL context.**
   - Create `sf::RenderWindow` (SFML 3 API) with an attached OpenGL context state.
   - Set `window.setActive(true)` before any GL call.

2. **Initialize ImGui + OpenGL3 backend.**
   - `ImGui::CreateContext()`, configure `ImGuiIO` (display size from window size,
     `BackendFlags`, font atlas — load custom font from `assets/fonts/`).
   - `ImGui_ImplOpenGL3_Init("#version 130")` (or appropriate GLSL string for
     desktop GL — to be confirmed in Phase 2 against ImGui docs via Context7).

3. **Pump SFML events into ImGui every frame.**
   SFML 3 changed the event API. The integration MUST be written for SFML 3 only:
   - `pollEvent()` returns `std::optional<sf::Event>`.
   - Events are accessed by type via `event.getIf<sf::Event::MouseMoved>()`,
     `getIf<sf::Event::KeyPressed>()`, `getIf<sf::Event::TextEntered>()`, etc.
   - Translate each SFML 3 event into the corresponding `ImGuiIO::AddXxxEvent(...)`
     call (mouse pos, mouse button, mouse wheel, key, modifier, text input, focus,
     resize). Scoped enums (e.g. `sf::Keyboard::Key::A`) must be mapped to ImGui keys.

4. **Drive the per-frame lifecycle in this exact order.**
   ```
   while (window.isOpen()) {
       while (auto event = window.pollEvent()) {
           InputBridge::handle(*event);     // SFML 3 -> ImGui
           if (event->is<sf::Event::Closed>()) window.close();
       }

       ImGui_ImplOpenGL3_NewFrame();
       InputBridge::newFrame(deltaTime, windowSize); // io.DeltaTime, io.DisplaySize
       ImGui::NewFrame();

       app.tick(deltaTime);                 // domain: timers, animations, autosave
       ui.draw(app);                        // UI layer: build screens

       ImGui::Render();
       window.clear(clearColor);
       window.setActive(true);
       ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
       window.display();
   }
   ```

5. **Shut down cleanly.**
   - `ImGui_ImplOpenGL3_Shutdown()`, `ImGui::DestroyContext()`, window close.

### 3.2 Why OpenGL3 (not SFML's renderer)

ImGui needs to issue real GL draw calls (textured vertex buffers). We use ImGui's
stock `imgui_impl_opengl3` backend — it's in `imgui-master/backends/` — and we give
it the SFML window's OpenGL context. SFML 3's own 2D renderer is still used for
non-ImGui things we need (loading textures via `sf::Texture` for logos and item
photos, audio playback via `sfml-audio` for the optional sound effects). We do
**not** try to render ImGui through SFML's renderer.

### 3.3 Textures shown inside ImGui

For the menu/item images, the logo on the receipt screen, etc., we load via
`sf::Texture` (file → GPU), then pass the OpenGL texture handle obtained from
`sf::Texture::getNativeHandle()` into `ImGui::Image((ImTextureID)(intptr_t)handle, size)`.
A `TextureCache` (in `src/platform/`) owns the `sf::Texture` lifetimes — the UI
layer never owns texture memory.

---

## 4. Domain layer — services, entities, repositories

The domain layer is organized in three concentric rings inside `src/domain/`:

- **Entities** — plain data classes with behavior (e.g. `Order::addItem`,
  `Order::total`, `MenuItem::isAvailable`). These hold OOP: inheritance (`User`
  → `Admin`/`Cashier`/`Kitchen`), operator overloading (`Money` type, `Receipt`
  stream output), templates (`Repository<T>`).
- **Services** — orchestrate use cases that touch multiple entities
  (`OrderService::placeOrder` deducts inventory, updates kitchen queue, appends to
  sales history). Services are the API the UI layer calls. They throw domain
  exceptions (`InsufficientStockException`, `InvalidCredentialsException`) which
  the UI catches and displays as popups.
- **Repositories** — the *only* code that opens files. One repo per persisted
  aggregate (`UserRepository`, `MenuRepository`, `OrderRepository`,
  `CustomerRepository`, `InventoryRepository`, `ReceiptRepository`,
  `SalesHistoryRepository`). A templated `BinaryRepository<T>` base provides
  generic `save/load/append/all/findById` over POD record structs; text-based
  repos extend a separate `TextRepository` base. This satisfies the "templates"
  OOP requirement cleanly.

Detailed class breakdown is in `03-class-design.md`.

---

## 5. UI layer — screens and shell

The UI layer is in `src/ui/` and is organized by screen:

```
ui/
  Shell.cpp            // sidebar, top bar, screen router, theme switch
  screens/
    SplashScreen.cpp
    LoginScreen.cpp
    DashboardScreen.cpp
    MenuScreen.cpp
    OrderScreen.cpp
    BillingScreen.cpp
    InventoryScreen.cpp
    TablesScreen.cpp
    KitchenScreen.cpp
    AnalyticsScreen.cpp
  widgets/
    Card.cpp           // small reusable ImGui widget helpers
    Toast.cpp          // notification popups
    Chart.cpp          // manual bar/pie draws via ImDrawList
    IconButton.cpp     // animated hover/press button
  theme/
    Theme.cpp          // dark/light palette, font loading, ImGuiStyle setup
  anim/
    Animation.cpp      // easing helpers (lerp, easeOutQuad) used by widgets
```

### 5.1 The "screen" pattern

Every screen exposes a single function:

```
namespace ui::screens {
    void DrawDashboard(AppContext& ctx);
    void DrawOrder(AppContext& ctx);
    // ...
}
```

`AppContext` holds references to all domain services + the current `Session` (logged-in
user) + the `Router` (which screen is active). The shell calls the active screen each
frame inside its main ImGui window. **Screens hold no business state** — only local
selection/scroll/animation state in `static` locals or in a small per-screen UI-state
struct kept inside `ui::state::*`.

### 5.2 Animations under immediate-mode

We do not have a real animation system. Instead, the `Animation` helper exposes:
- `float easeOutQuad(float t)`
- `float smoothStep(float a, float b, float t)`
- a `Tween` struct (`current`, `target`, `speed`) that the widget updates each frame
  using `io.DeltaTime`.

Hover scaling, sidebar expand/collapse, toast slide-in, splash fade — all driven by
per-frame interpolation on these values. No threads, no timelines.

### 5.3 Charts (no ImPlot)

`widgets/Chart.cpp` exposes:
- `BarChart(const std::vector<BarDatum>& data, ImVec2 size)`
- `PieChart(const std::vector<PieSlice>& data, float radius)`
- `LineChart(const std::vector<float>& series, ImVec2 size)` (for the dashboard
  revenue trend).

All three are drawn manually using `ImGui::GetWindowDrawList()` →
`AddRectFilled`, `AddLine`, `PathArcTo`/`PathFillConvex`. Inputs come from
`AnalyticsService` (which reads `sales_history.dat`).

---

## 6. Application entry and lifecycle

```
main()
  └─ App app;                 // constructs services, repos, loads data
       app.run();             // owns the SFML window + ImGui context
                              // runs the per-frame loop in §3.1
                              // shuts down ImGui, saves dirty data, closes window
```

- `App::run()` is the *only* place SFML and ImGui are touched.
- `App` owns: `Platform` (window + integration), `Services` (struct of all services),
  `Session` (current user), `Router` (active screen + history), `Theme`.
- Crash safety: services that mutate persisted data write through repositories that
  use a **write-temp-then-rename** pattern (write to `*.tmp`, then `std::rename`)
  so a power loss can't corrupt the live file.

---

## 7. Module interaction (a worked example)

**Use case:** Cashier places an order.

1. UI: `OrderScreen` shows the menu grid (read from `MenuService::all()`).
2. UI: cashier taps items — local `DraftOrder` UI state appends `MenuItem` IDs and
   quantities. *No domain mutation yet.*
3. UI: cashier taps "Place order".
4. UI calls `orderService.place(draft, session.currentUser, optionalCustomerId)`.
5. `OrderService::place`:
   - validates the draft (throws `EmptyOrderException` if empty),
   - asks `InventoryService::canFulfill(draft)` — throws `InsufficientStockException`
     otherwise,
   - constructs an `Order` (auto-numbered, timestamped),
   - calls `inventoryService.deduct(order)`,
   - calls `kitchenService.enqueue(order)`,
   - calls `orderRepository.append(order)`,
   - calls `salesHistoryRepository.append(SalesRecord{order})`,
   - returns the persisted `Order`.
6. UI receives the result, builds a `Receipt` via `billingService.makeReceipt(order)`,
   navigates to `BillingScreen`, fires a "success" toast and a register-bell sound.
7. On exception, UI catches it and shows a red toast (see `widgets/Toast`).

This is the standard flow every feature follows. The UI is dumb. The services own
the truth. The repositories own the bytes.

---

## 8. Auth and roles

Three roles, modeled as an inheritance hierarchy in `src/domain/auth/`:

```
User (abstract)
 ├─ Admin       // full access
 ├─ Cashier     // orders + billing + tables + customers (read-only menu, read-only inv)
 └─ Kitchen     // kitchen display only, can flip order status
```

`User` declares pure-virtual `bool can(Capability) const`. Each subclass overrides it.
The UI shell consults `session.currentUser->can(Capability::EditMenu)` to enable or
hide sidebar entries. This satisfies the **polymorphism + abstraction** OOP requirements.

Credentials stored in `data/users.dat` (binary, fixed-width fields, password hashed
with a simple salted hash — see `08-file-schemas.md`). Login screen uses ImGui's
`ImGuiInputTextFlags_Password` for masked input. "Remember login" writes a token to
`data/settings.txt`.

---

## 9. Theming and assets

- Dark theme is the default; light theme is a runtime switch (`Theme::apply(Dark|Light)`
  builds an `ImGuiStyle` and calls `ImGui::GetStyle() = style`).
- Fonts loaded from `assets/fonts/` via `ImFontAtlas::AddFontFromFileTTF` before the
  first `NewFrame`.
- Icons: we use ImGui's text + a font-icon TTF (Font Awesome free, included in
  `assets/fonts/`) — no separate icon library needed.
- Images: logo, menu item photos load from `assets/img/` via `sf::Texture`, displayed
  with `ImGui::Image` (see §3.3).
- Sounds (optional): `assets/sfx/*.wav` loaded via `sf::SoundBuffer` and played via
  `sf::Sound`.

---

## 10. Build system (Phase 2 — noted here only)

CMake (no `CMakeLists.txt` in this phase). Targets the in-folder `SFML-3.1.0` and
`imgui-master`. Compiles ImGui sources + the chosen backend (`imgui_impl_opengl3.cpp`)
as part of our own target — there is no separate ImGui library to link. Links the
SFML 3 libs (graphics, window, system, audio). On Windows/MinGW, the matching SFML 3
runtime DLLs must be copied next to the `.exe` so it runs on double-click.

> **Step-0 verification flag (for `10-roadmap.md`):** the vendored `SFML-3.1.0/lib/`
> contains `.lib` files (Windows-import style) and `bin/` contains `sfml-*-3.dll`,
> not the `lib*.a` archives MinGW typically links against. Phase 2 Step 0 must
> confirm whether MinGW-w64 can link these `.lib` files directly, or whether SFML
> must be rebuilt from source for GCC (or whether MSVC must be used as the
> toolchain instead). Planning continues as if the toolchain is MinGW-w64 as
> specified in the brief; this check happens before any feature work.

---

## 11. Key design decisions and rationale (one-liners)

| # | Decision | Why |
|---|----------|-----|
| 1 | Strict domain / UI split | Preserves OOP grade under immediate-mode UI |
| 2 | Repositories own ALL file I/O | One place to debug persistence; testable |
| 3 | Manual ImGui ↔ SFML 3 integration via OpenGL3 backend | Mandated; no binding |
| 4 | SFML used for window + texture loading + audio only | Avoid double rendering |
| 5 | Charts hand-drawn with `ImDrawList` | No ImPlot dependency |
| 6 | Animations = per-frame lerp using `io.DeltaTime` | Fits immediate-mode model |
| 7 | Templated `BinaryRepository<T>` for POD records | Demonstrates templates |
| 8 | Write-temp-then-rename for binary writes | Crash-safe persistence |
| 9 | Domain exceptions caught by UI → toasts | Demonstrates exception handling |
| 10 | "Smart" features = file-data heuristics | No ML in scope |

---

## 12. What is explicitly OUT of scope

- No SQL / no SQLite / no database of any kind.
- No real networking, no real payment integration, no real printing — receipts go to
  a text file and a PDF-style text export; "online payment" simulation writes a
  pseudo-transaction line into a file.
- No ImPlot, no ImGui-SFML binding, no Qt, no wxWidgets.
- No multithreading. The app is single-threaded.
- No machine learning. Any "smart" feature is a counting heuristic over files.

---

*End of `01-architecture.md`.*
