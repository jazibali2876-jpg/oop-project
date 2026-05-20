# 10 — Phase 2 Build Roadmap

> Phase 1 planning document. This is the ordered build plan we follow in Phase 2.
> Every step lists its **dependency** (what must already be true), its **deliverable**
> (what exists after the step), and its **verification** (how we know it works).
>
> Rules carried in from earlier docs:
> - All new code lives under `src/`, `assets/`, `data/`. Never inside `SFML-3.1.0/`,
>   `imgui-master/`, or `mingw64/`.
> - Domain layer never includes ImGui/SFML headers.
> - `src/platform/` is the only folder that includes both.
> - Repositories are the only code that opens `fstream`s.

---

## Step 0 — Toolchain & SFML linkage verification (BLOCKER)

**Why first:** the planning brief explicitly mandates this. The vendored
`SFML-3.1.0/lib/` contains `.lib` import files (not the `lib*.a` archives MinGW
usually links against). We need to confirm what actually links before any code work.

**Dependency:** none (clean environment).

**Plan (in priority order):**

1. **Attempt MinGW + `.lib` direct linking first.**
   - Use the bundled `./mingw64/bin/g++.exe` (verify with `g++ --version`).
   - Write a 10-line `link-probe.cpp` (NOT committed long-term — for this step only)
     that includes `<SFML/Window.hpp>` and opens an `sf::Window`.
   - Compile with `-I ../SFML-3.1.0/include` and link with `-L ../SFML-3.1.0/lib`
     against `-lsfml-graphics -lsfml-window -lsfml-system`.
   - Run it. If a window opens and closes cleanly: **MinGW + `.lib` works → proceed**.

2. **Fallback A — rebuild SFML 3.1.0 from source with the bundled GCC.**
   - Pull SFML 3.1.0 source (CMake build), point `CMAKE_CXX_COMPILER` at the bundled
     `mingw64/bin/g++.exe`, build statically + dynamically.
   - Replace `SFML-3.1.0/lib/` and `SFML-3.1.0/bin/` with the produced artifacts
     **in a sibling folder** (`SFML-3.1.0-mingw/`) — do NOT modify the vendored
     `SFML-3.1.0/` per the no-touch rule.
   - Re-run the link probe against the rebuilt SFML.

3. **Fallback B — switch to MSVC (last resort).**
   - Only if both above fail or eat more than ~1 day. Document the decision in
     `docs/decisions/D001-toolchain.md`. The rest of this roadmap is unchanged
     except CMake's generator (`-G "Visual Studio 17 2022"` instead of `-G "MinGW Makefiles"`).

**Deliverable:** a working hello-world SFML 3 window, and a single line in
`docs/decisions/D001-toolchain.md` stating "MinGW + .lib" or "MinGW + rebuilt SFML"
or "MSVC". Delete the link-probe file when done.

**Verification:** the probe `.exe` runs on double-click in `dist/` after the required
DLLs are next to it.

> User preference acknowledged: stay on MinGW as long as possible — only fall
> back to MSVC if both MinGW paths fail.

---

## Step 1 — Verify SFML 3.1 & ImGui APIs via Context7 before any feature coding

**Why:** SFML 3.x and ImGui master both move; this project locks against specific
API shapes (`std::optional<sf::Event>`, `event.getIf<T>`, scoped key enums,
`ImGui_ImplOpenGL3_Init`, `ImGuiIO::AddXxxEvent`). Hard-coding from memory will
cause a long debugging tail.

**Dependency:** Step 0 complete (we know how to compile).

**Action:** for each of the following symbols, fetch the official API shape via
Context7 and paste a one-line confirmation into `docs/api-pins/sfml3.md` and
`docs/api-pins/imgui.md`:
- SFML 3: `sf::RenderWindow`, `pollEvent()`, every `sf::Event::*` we use,
  `sf::Keyboard::Key`, `sf::Mouse::Button`, `sf::Texture::getNativeHandle()`,
  `sf::SoundBuffer`, `sf::Sound`, `sf::Clock`.
- ImGui: `ImGui::CreateContext`, `ImGuiIO::AddMousePosEvent`, `AddMouseButtonEvent`,
  `AddMouseWheelEvent`, `AddKeyEvent`, `AddInputCharacter`, `AddFocusEvent`,
  `ImGuiKey`, `ImGui_ImplOpenGL3_Init`, `_NewFrame`, `_RenderDrawData`, `_Shutdown`,
  `ImDrawList::AddRectFilled`, `PathArcTo`, `PathFillConvex`, `ImGui::Image`.

**Deliverable:** two short reference files in `docs/api-pins/` with verified
signatures.

**Verification:** zero "guessed" symbol use during Steps 3–4.

---

## Step 2 — Dependency / setup verification script (per user request)

**Why:** catch broken environments BEFORE we start writing features. Anyone cloning
the repo should be able to run one script and know what's missing.

**Dependency:** Steps 0–1 complete.

**Deliverable:** `tools/verify-setup.ps1` (PowerShell — Windows) and a mirrored
`tools/verify-setup.bat` for double-click users. The script performs, in order:

1. **Compiler check.**
   - `g++ --version` resolves to `./mingw64/bin/g++.exe` (compare path).
   - GCC major version is `>= 13` (the version shipped in `mingw64/`).
2. **SFML headers + libs present.**
   - `SFML-3.1.0/include/SFML/Graphics.hpp` exists.
   - `SFML-3.1.0/lib/sfml-graphics.lib` (or `.a` if rebuilt) exists.
3. **SFML linkage smoke compile.**
   - Compile `tools/probes/sfml_probe.cpp` to `build/probe_sfml.exe`. Compile must
     return 0. Don't run it — compile success alone is the check.
4. **DLL presence next to the build target.**
   - `SFML-3.1.0/bin/sfml-graphics-3.dll`, `sfml-window-3.dll`, `sfml-system-3.dll`,
     `sfml-audio-3.dll` all exist. (Plus `openal32.dll` if shipped with SFML.)
5. **ImGui sources present.**
   - `imgui-master/imgui.cpp`, `imgui-master/backends/imgui_impl_opengl3.cpp`,
     `imgui-master/backends/imgui_impl_opengl3_loader.h` all exist.
6. **Font assets present.**
   - `assets/fonts/Inter-Regular.ttf`, `Inter-Bold.ttf`, `fa-solid-900.ttf` all exist.
     (Files are vendored locally — see Step 5 below.)
7. **Asset paths sane.**
   - `assets/img/logo.png` exists.
   - `assets/img/menu/` is a folder (may be empty pre-seed).
8. **`data/` writable.**
   - Attempt to write `data/.write_probe`, delete on success. Print failure
     diagnostics on permission error.
9. **CMake present.**
   - `cmake --version` resolves; major version ≥ 3.20.
10. **Summary.**
    - Print a green `[OK]` per check or a red `[FAIL]` with the exact remediation
      ("re-run Step 0 fallback", "drop the .ttf file into assets/fonts/", etc.).
    - Exit non-zero if any check fails so CI / pre-build hooks can gate on it.

**Verification:** run with both healthy and intentionally-broken setups (rename
a DLL temporarily) and confirm both pass and fail report cleanly.

> Once this script exists, every subsequent step's first line becomes "run
> `verify-setup` first".

---

## Step 3 — CMake skeleton

**Why:** without a build system, we can't move from one-off `g++` probes to a real
project layout.

**Dependency:** Step 2 green.

**Deliverable:** `CMakeLists.txt` at the repo root that:
- Sets `CMAKE_CXX_STANDARD 17`, `CMAKE_CXX_STANDARD_REQUIRED ON`.
- Defines `SFML_DIR = ${CMAKE_SOURCE_DIR}/SFML-3.1.0` and creates IMPORTED
  targets for `sfml-graphics`, `sfml-window`, `sfml-system`, `sfml-audio`.
- Defines `IMGUI_DIR = ${CMAKE_SOURCE_DIR}/imgui-master` and compiles the
  ImGui core + the OpenGL3 backend as part of our executable (no separate static
  lib needed).
- Adds `add_executable(smart_pos ...)` with empty `main.cpp` only.
- Adds a post-build custom command that copies the required SFML DLLs from
  `SFML-3.1.0/bin/` into `$<TARGET_FILE_DIR:smart_pos>`.
- Sets `set_target_properties(smart_pos PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist)`
  so the `.exe` lands somewhere predictable.

**Verification:**
```
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```
produces an empty `dist/smart_pos.exe` with all DLLs sitting next to it.

---

## Step 4 — Smoke test: empty ImGui window inside an SFML window

**Why:** the planning brief explicitly requires this BEFORE any feature work. It
proves the manual integration design works.

**Dependency:** Step 3 complete.

**Deliverable:** `src/main.cpp` + minimal `src/platform/*` that:
- Opens an `sf::RenderWindow` (SFML 3 API, no deprecation warnings).
- Initializes ImGui + the OpenGL3 backend.
- Runs the loop in `01-architecture.md` §3.1 (event pump → newFrame → build → render → display).
- Draws a single `ImGui::Begin("Smoke Test") / ImGui::Text("Hello, Smart POS") / ImGui::End()`.

**Verification:** `dist/smart_pos.exe` shows an SFML window with an ImGui panel
inside it. Mouse hovers/clicks register (`ImGuiIO::WantCaptureMouse` flips true
when over the panel). Keyboard input goes into a tiny `ImGui::InputText` test box.
Window close button works. No console error spam.

If anything in here is wrong, FIX IT NOW — going further with a broken integration
will waste a week.

---

## Step 5 — Vendor fonts + first asset drop

**Why:** the dependency-verify script requires these to be on disk; later UI work
needs them anyway.

**Dependency:** none (parallel with Steps 3–4 if convenient).

**Deliverable:**
- `assets/fonts/Inter-Regular.ttf` — Inter Regular weight.
- `assets/fonts/Inter-Bold.ttf` — Inter Bold weight.
- `assets/fonts/fa-solid-900.ttf` — Font Awesome Free Solid (CC-BY 4.0).
- `assets/img/logo.png` — placeholder logo.
- `assets/img/login_bg.png` — placeholder.
- `assets/img/menu/.gitkeep` — folder marker.
- `assets/img/menu/burger.png`, `bbq_chicken.png`, ... — placeholders (one per seed item).
- `docs/decisions/D002-fonts.md` — license notes for Inter (OFL) and Font Awesome Free.

**Verification:** `verify-setup` step 6 turns green.

> No runtime downloads of these assets — they are committed to the repo. Per user
> preference, the build must remain offline-capable after initial setup.

---

## Step 6 — Domain skeleton: common + auth + persistence base

**Why:** every other domain package depends on these.

**Dependency:** Step 4 (we have a runnable shell).

**Deliverable (in this order, with a unit test or scratch `main` per piece):**
1. `src/domain/common/Money.{h,cpp}` (+ operator overload tests in `tools/scratch/`).
2. `src/domain/common/DateTime.{h,cpp}`.
3. `src/domain/common/Id.h`, `Capability.h`, `Exceptions.h`, `Result.h`.
4. `src/domain/persistence/TextRepository.{h,cpp}`.
5. `src/domain/persistence/BinaryRepository.h` (template).
6. `src/domain/persistence/FilePaths.h` (constexpr table of every file).
7. `src/domain/auth/User.{h,cpp}` + `Admin/Cashier/Kitchen.{h,cpp}`.
8. `src/domain/auth/PasswordHash.{h,cpp}`.
9. `src/domain/auth/Session.{h,cpp}` + `AuthService.{h,cpp}` + `UserRepository.{h,cpp}`.

**Verification:** a scratch executable in `tools/scratch/` seeds an admin user,
writes/reads `data/users.dat`, logs in successfully, and calls
`session.user().can(Capability::EditMenu)` returning the expected value.

---

## Step 7 — Theme + Shell + Splash + Login UI

**Why:** the first user-facing screens. Tightens the integration layer under real
event load.

**Dependency:** Step 6 (`AuthService`, `Session`).

**Deliverable:**
- `src/ui/theme/Theme.{h,cpp}` + `Fonts.{h,cpp}` + `Icons.h`.
- `src/ui/Shell.{h,cpp}` (sidebar + top bar).
- `src/ui/Router.{h,cpp}`.
- `src/ui/screens/SplashScreen.cpp`.
- `src/ui/screens/LoginScreen.cpp`.
- `src/platform/TextureCache.{h,cpp}` (used for splash logo).

**Verification:** start the app → see splash for ~1.5s → fade to Login → log in
as `admin / admin123` → shell appears (empty content area). Theme toggle in top
bar switches dark/light at runtime.

---

## Step 8 — Menu service + Menu screen

**Why:** the simplest CRUD slice — proves the domain→UI pattern end to end.

**Dependency:** Step 7.

**Deliverable:** `MenuItem`, `MenuService`, `MenuRepository`, plus `MenuScreen.cpp`
and a few menu-item placeholder images.

**Verification:** Admin can add, edit, delete, search items. Restart and the items
persist (`data/menu.txt` updated). Cashier login sees the same screen read-only
(Edit/Delete hidden by `can(EditMenu)`).

---

## Step 9 — Inventory + Recipes + Inventory screen

**Why:** required by the order flow's `canFulfill`/`deduct`.

**Dependency:** Step 8.

**Deliverable:** `Ingredient`, `Recipe`, `InventoryService`, `InventoryRepository`,
`RecipeRepository`, `InventoryScreen.cpp` (including the low-stock highlight and
the usage BarChart placeholder reading dummy data).

**Verification:** Adjust stock by hand → the dashboard low-stock card (Step 11)
reads it. `InventoryService::canFulfill` returns the right value for a hand-crafted
draft order.

---

## Step 10 — Order + Billing + Kitchen pipeline

**Why:** the central business workflow.

**Dependency:** Steps 8–9.

**Deliverable (in order):**
1. `OrderItem`, `Order`, `OrderRepository`, `SalesHistoryRepository`.
2. `OrderService::place / cancel / setStatus / active / byId`.
3. `OrderScreen.cpp` — draft cart, totals, place button.
4. `Receipt`, `BillingService`, `ReceiptRepository`, `BillingScreen.cpp`.
5. `KitchenTicket`, `KitchenService`, `KitchenScreen.cpp`.

**Verification:** Cashier places an order → inventory deducts → receipt file
appears in `data/receipts/` → Kitchen sees the ticket → Kitchen advances it
through Preparing → Ready → Served and the ticket disappears. Restart the app and
the active orders reload from `data/orders.dat` correctly.

---

## Step 11 — Customers + Tables + Dashboard cards & first chart

**Why:** rounds out the day-to-day cashier workflow + lands the first hand-drawn chart.

**Dependency:** Step 10.

**Deliverable:**
- `Customer`, `LoyaltyRule`, `CustomerService`, `CustomerRepository`, `LoyaltyRepository`.
- Phone-search + register flow inside `OrderScreen`.
- `Table`, `TableService`, `TableRepository`, `TablesScreen.cpp`.
- `DashboardScreen.cpp` with cards + LineChart (revenue last 7 days, read from
  `sales_history.dat`).
- `widgets/Chart.cpp` (LineChart implementation via `ImDrawList`).

**Verification:** Place 7 orders across 7 days → dashboard's revenue line draws
correctly. Apply loyalty → points show on customer record after restart.

---

## Step 12 — Analytics + remaining charts

**Why:** completes the Admin power-user surface.

**Dependency:** Steps 10–11.

**Deliverable:**
- `SalesRecord`, `Report`, `AnalyticsService`.
- `AnalyticsScreen.cpp` with all three charts (Bar, Pie, Line) and the employee
  performance table.
- CSV export.

**Verification:** With seeded data covering ≥30 days, every chart and every metric
on the screen visibly reflects file contents (sanity-check by hand for at least
one day).

---

## Step 13 — Polish, animations, sounds, toasts, "smart" heuristics

**Why:** lifts the project from "works" to "viva-impressive".

**Dependency:** all functional steps done.

**Deliverable:**
- `ui/anim/Animation.{h,cpp}` + `Tween`.
- Per-screen polish: hover scale on cards, sidebar collapse tween, toast slide-in,
  splash fade.
- Optional sound effects via `AudioPlayer` (toggle in settings).
- Smart features (all file-heuristic, NOT ML):
  - **Popular-item detection** — `Report::topItems` already exists; surface as a
    "Customer also ordered" hint on `OrderScreen`.
  - **Predictive low-stock** — per-day usage avg from `sales_history.dat`; show an
    "out in N days" estimate next to low-stock ingredients.
- Keyboard shortcuts (`Ctrl+N` new order, `Ctrl+P` print, etc.).

**Verification:** demo the polish — animations feel smooth (60 fps), toasts queue
sensibly, smart hints look correct against hand-checked file contents.

---

## Step 14 — Release build, DLL packaging, run-from-double-click verification

**Why:** what the grader actually opens.

**Dependency:** Step 13 stable.

**Deliverable:**
- `cmake --build build --config Release`.
- `dist/` populated:
  - `smart_pos.exe`
  - `sfml-graphics-3.dll`, `sfml-window-3.dll`, `sfml-system-3.dll`, `sfml-audio-3.dll`
  - `openal32.dll` (from SFML's `bin/`)
  - **MinGW runtime DLLs** copied from `mingw64/bin/`: typically
    `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll` (confirm
    actual names in Phase 2 — they depend on the GCC build).
  - `assets/` folder copied wholesale.
  - `data/` folder pre-seeded with defaults.
- A short `README.md` at the repo root with "double-click `dist/smart_pos.exe`".

**Verification:**
1. Zip `dist/` and unzip on a *clean machine* (or a Windows VM without MinGW on PATH).
2. Double-click `smart_pos.exe`. The app must run end-to-end: login as admin,
   place an order, print a receipt, view dashboard.
3. No "missing DLL" dialog.

> CMake post-build commands automate the SFML DLL copy. The MinGW runtime DLL copy
> is a separate post-build step we explicitly add (the verify-setup script's check
> 4 covers SFML DLLs, but the MinGW runtimes ride along with the toolchain and need
> to be packaged manually).

---

## Step 15 — Final report + viva prep

**Why:** the grade is on the report and the viva, not just the code.

**Dependency:** Step 14 shipped.

**Deliverable:**
- `docs/final-report.md` (filled out from the stub) covering: architecture,
  OOP demonstrations (mirror `04-oop-map.md`), file schemas (`08-file-schemas.md`),
  screenshots, build instructions, known limitations.
- `docs/screenshots/` — captures of every screen.
- `docs/viva-cheatsheet.md` — the "defense flow" from `04-oop-map.md` §4.

**Verification:** dry-run the viva walkthrough end-to-end on a clean machine
using only the report + the cheat sheet.

---

## Cross-cutting dependency graph (Phase 2 at a glance)

```
[0 Toolchain] ──> [1 API pins] ──> [2 verify-setup] ──> [3 CMake] ──> [4 Smoke ImGui] ──┐
                                                                                        │
[5 Vendor fonts/assets] ──────────────────────────────────────────────────────> [7 Theme/Shell/Login]
                                                                                        │
                                                              [6 Domain skeleton] ──────┤
                                                                                        │
                                                                          [7 Theme/Shell/Login]
                                                                                        │
                                                                                [8 Menu]
                                                                                        │
                                                                                [9 Inventory]
                                                                                        │
                                                                                [10 Orders/Billing/Kitchen]
                                                                                        │
                                                                                [11 Customers/Tables/Dashboard]
                                                                                        │
                                                                                [12 Analytics]
                                                                                        │
                                                                                [13 Polish]
                                                                                        │
                                                                                [14 Package & ship]
                                                                                        │
                                                                                [15 Report & viva]
```

---

## Risk register (short)

| Risk | Likelihood | Mitigation |
|---|---|---|
| `.lib` files don't link with MinGW | Medium | Step 0 fallbacks (rebuild SFML, or switch to MSVC) |
| ImGui OpenGL backend init fails | Low–Med | Step 1 API pins; verified manually in Step 4 before any features |
| Binary record struct padding drifts between compilers | Low (single-toolchain project) | `static_assert(sizeof(X) == N)` per record (`08-file-schemas.md` §13) |
| Per-frame chart drawing too slow with large `sales_history.dat` | Low (file is small — ≤1MB for thousands of orders) | If it ever bites, cache `Report` and recompute only on filter change |
| Asset path breakage when running outside `dist/` | Med | All paths relative to executable directory (`util::Files::exePath`) |
| Receipt files overwrite each other if id collides | Low | Order ids are monotonically increasing in `OrderRepository::nextId` |

---

## Stop-the-line conditions (anything below halts Phase 2 until I check in)

1. Step 0 falls through all three fallbacks and we cannot link SFML at all.
2. Step 4 (smoke test) cannot get input events to register in ImGui after a day
   of debugging — the manual integration design needs a second pass.
3. We discover any feature in the brief that genuinely cannot be done with file
   handling only (e.g. real-time multi-station kitchen sync) — surface it,
   downgrade or remove that feature.
4. Padding/size mismatches on the binary records start corrupting data on
   restarts — pin format and add static asserts before continuing.

---

*End of `10-roadmap.md`.*
