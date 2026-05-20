# Smart Restaurant POS

A modern, offline-first Point-of-Sale desktop application for restaurants —
a final-year university Object-Oriented Programming project.

Built with **C++17**, **SFML 3.1**, **Dear ImGui**, **CMake**, and **MinGW-w64 GCC**.
Storage is **plain C++ file handling** (text + binary), no database.

> Looks and feels like commercial POS software (Toast / Square style), with a
> dark-by-default UI, hand-drawn charts, animated transitions, and a real
> role-based auth system.

## Run it now (Windows)

1. Clone or download this repo.
2. Open the `dist/` folder.
3. Double-click `smart_pos.exe`.

Default sign-in: **`admin` / `admin123`**.

The first launch creates `dist/data/` (menu, inventory, tables, users, etc.)
automatically — there is nothing to configure.

See [`dist/HOW_TO_RUN.txt`](dist/HOW_TO_RUN.txt) for details and troubleshooting.

## Features

- **Auth** — three roles (Admin / Cashier / Kitchen) with per-capability access control.
- **Dashboard** — today's revenue, active orders, low-stock alerts, 7-day revenue chart, top-selling item.
- **Menu management** — full CRUD over menu items with category filter + search; admin-only.
- **Order system** — visual menu grid + cart with auto totals, tax, discount, customer lookup by phone, and prep-time estimate.
- **Billing & receipts** — cash / card / online-sim; thermal-style 44-column receipts; save + PDF-style export.
- **Inventory** — ingredient stock with low-stock highlight; recipes auto-deduct stock when an order is placed.
- **Tables** — color-coded grid (Free / Occupied / Reserved); tap to reserve, occupy, or free.
- **Kitchen display** — three lanes (Pending / Preparing / Ready) with one-tap status advance and live elapsed-time per ticket.
- **Analytics** — today / week / month reports with hand-drawn bar, pie, and line charts (no ImPlot — pure `ImDrawList`).

## Repository layout

| Path | What's inside |
|---|---|
| [`src/`](src/) | All application source: `platform/`, `domain/` (the OOP-graded layer), `ui/`, `util/`. |
| [`planning/`](planning/) | The 10 design documents (architecture, class design, OOP map, UML, DFDs, schemas, wireframes, roadmap). |
| [`presentation/`](presentation/) | `RestaurantPOS_Demo.pptx` and the Python script that generated it. |
| [`dist/`](dist/) | The runnable Windows bundle — `smart_pos.exe` + every required DLL + `assets/` + `data/`. |
| [`assets/`](assets/) | Vendored fonts (Inter regular/bold, Font Awesome 6 free solid) and image folders. |
| [`tools/`](tools/) | `preflight_check.ps1` and a small SFML link probe. |
| `CMakeLists.txt` | The single-file build description. |

Not in the repo (gitignored — see *Build from source* below):
- `SFML-3.1.0/`, `imgui-master/`, `mingw64/`, `third_party/` — vendored toolchain + libraries used during the build.
- `build/`, intermediate `*.o` / `*.obj` files.

## Architecture in one paragraph

A strict **domain / UI split**. The **domain layer** (`src/domain/`) is plain C++ —
no ImGui, no SFML — and holds every entity, service, repository, and all business
state. The **UI layer** (`src/ui/`) is immediate-mode ImGui screens that read from
the domain layer and call into its services; UI holds only transient state
(selection, scroll, animation timers). A small **platform layer** (`src/platform/`)
is the only place that includes both ImGui and SFML — it hand-writes the
ImGui ↔ SFML 3 integration (manual `sf::Event` → `ImGuiIO` translation, driving the
bundled `imgui_impl_opengl3` backend on SFML's OpenGL context). This preserves the
OOP grade under ImGui's procedural style.

See [`planning/01-architecture.md`](planning/01-architecture.md) for the full picture
and [`planning/04-oop-map.md`](planning/04-oop-map.md) for the OOP-concept → class map.

## Build from source

You need to supply, alongside the repo (these are gitignored to keep the repo small):

| Where | What |
|---|---|
| `./SFML-3.1.0/` | SFML 3.1.0 distribution (any flavor; used as a vendored snapshot — see notes). |
| `./imgui-master/` | Dear ImGui (master branch). |
| `./mingw64/` | MinGW-w64 GCC 14.x (UCRT / POSIX / SEH). |
| `./third_party/` | A portable CMake build (used to invoke the build below) and a from-source MinGW build of SFML — see below. |

### Why SFML must be rebuilt for MinGW

The vendored `SFML-3.1.0/` ships as an **MSVC build** (`.lib` import libraries with
`.pdb` debug-symbol files). MinGW cannot link those directly — MSVC's C++ name
mangling is incompatible with MinGW's Itanium ABI. The build expects an **MSVC ↔
MinGW workaround**: SFML 3.1.0 source rebuilt with the bundled MinGW into
`./third_party/SFML-mingw/`, producing proper `lib*.a` archives plus the matching
`*.dll` runtime.

The CMake script consumes `./third_party/SFML-mingw/lib/cmake/SFML/SFMLConfig.cmake`
via `find_package(SFML 3 CONFIG)`.

### Build command (PowerShell or Git Bash)

```bash
# from the project root, with the toolchain folder on PATH:
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

The output `smart_pos.exe` + all runtime DLLs + `assets/` lands in `./dist/`,
ready to run.

## Tech stack notes

- **C++17** throughout; no C++20 features required.
- **SFML 3.1.0** — note the breaking changes from SFML 2.x: `pollEvent()` returns
  `std::optional<sf::Event>`; events are read via `event.getIf<sf::Event::T>()`;
  enums are scoped (`sf::Keyboard::Key::A`).
- **Dear ImGui** — used in immediate mode with the bundled `imgui_impl_opengl3`
  backend. There is **no ImGui-SFML binding**; the integration is hand-written
  in `src/platform/`.
- **Charts** are drawn manually using `ImDrawList` primitives (`AddRectFilled`,
  `AddLine`, `PathArcTo`/`PathFillConvex`). No ImPlot dependency.
- **CMake 3.30** — vendored portable copy under `third_party/cmake-3.30.5-windows-x86_64/`.

## OOP demonstrated

Every required concept lives somewhere concrete:

| Concept | Where to point in the code |
|---|---|
| Abstraction / pure-virtual | [`src/domain/auth/User.h`](src/domain/auth/User.h) |
| Inheritance / polymorphism | `User → Admin / Cashier / Kitchen` in [`src/domain/auth/`](src/domain/auth/) |
| Templates | [`src/domain/persistence/BinaryRepository.h`](src/domain/persistence/BinaryRepository.h), `Id<Tag>` |
| Operator overloading | [`src/domain/common/Money.h`](src/domain/common/Money.h), `Receipt::operator<<`, `Order::operator+=` |
| Encapsulation | [`src/domain/order/Order.h`](src/domain/order/Order.h) — private items, mutation only via methods |
| Exception handling | [`src/domain/common/Exceptions.h`](src/domain/common/Exceptions.h) — `DomainException` base + 8 subclasses |
| STL containers | `vector`, `map`, `unordered_map`, `array<int,24>`, `optional`, `unique_ptr` |
| Header / source split | Every non-template class |

Full mapping with viva-prep one-liners: [`planning/04-oop-map.md`](planning/04-oop-map.md).

## Acknowledgments

- **Dear ImGui** by Omar Cornut and contributors — MIT.
- **SFML** by the SFML team — zlib/png.
- **Inter** typeface by Rasmus Andersson — SIL Open Font License 1.1.
- **Font Awesome Free 6** — CC BY 4.0 (icons) + SIL OFL 1.1 (fonts).
