# 09 — GUI Wireframes (ASCII)

> Phase 1 planning document. ASCII wireframes for every screen. They are reference
> sketches, not pixel mocks — Phase 2 builds them as ImGui widgets in
> `src/ui/screens/*`.
>
> Layout conventions:
> - The main window is **1280×800** at startup, resizable to ≥ 1024×640.
> - All screens (except Splash and Login) share the **Shell**: a 220-px sidebar on
>   the left and a 56-px top bar across the top.
> - "[…]" denotes a button. "(…)" denotes a tab pill. "──┐" denotes a card.
> - "📊" / "📈" markers point to **manually drawn charts** (`ImDrawList`).
> - All animations are per-frame interpolation (see `01-architecture.md` §5.2).

---

## 0. Shell — sidebar + top bar (every authenticated screen has this)

```
┌──────────────────────────────────────────────────────────────────────────────┐
│ ☰  Smart Restaurant POS               🔍 [ search anything... ]   🌙  🔔  👤 │  ← Top bar (56px)
│                                                          14:32:08  ●  nadia  │
├────────────┬─────────────────────────────────────────────────────────────────┤
│            │                                                                 │
│ 🏠 Home    │                                                                 │
│ 🍔 Menu    │                                                                 │
│ 🧾 Order   │                       SCREEN CONTENT                            │
│ 💳 Billing │                                                                 │
│ 📦 Stock   │                                                                 │
│ 🪑 Tables  │                                                                 │
│ 👨 Kitchen │                                                                 │
│ 📊 Reports │                                                                 │
│            │                                                                 │
│ ────────── │                                                                 │
│ ⚙  Settings│                                                                 │
│ ↩  Logout  │                                                                 │
│            │                                                                 │
└────────────┴─────────────────────────────────────────────────────────────────┘
    220px                                                                  rest
```

- Sidebar items hidden by `User::can(...)` — Kitchen sees only `Kitchen` + `Logout`.
- Top-right shows real-time clock, theme toggle (🌙/☀), notifications bell with toast count, and
  current user with a dropdown for `Logout`.
- The hamburger ☰ collapses the sidebar to icons-only (per-frame width tween).

---

## 1. Splash Screen

Shown for ~1.5 seconds on launch, then fades into Login.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                                                                              │
│                                                                              │
│                                                                              │
│                              ┌──────────────┐                                │
│                              │              │                                │
│                              │     🍴       │   ← logo (sf::Texture)         │
│                              │              │                                │
│                              └──────────────┘                                │
│                                                                              │
│                       Smart Restaurant POS                                   │
│                                                                              │
│                       ▰▰▰▰▰▰▰▰▰░░░░░░░░ 60 %                                │  ← progress bar (tween)
│                                                                              │
│                       Loading menu, inventory…                               │
│                                                                              │
│                                                                              │
│                                                                              │
│                       v1.0   ·   © 2026                                      │
└──────────────────────────────────────────────────────────────────────────────┘
```

- Logo is an `ImGui::Image` from `TextureCache`.
- Progress bar is `ImGui::ProgressBar`, advanced from the per-frame loader as each
  service finishes booting (auth → menu → inventory → customers → analytics).

---

## 2. Login Screen

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                                                                              │
│         ┌──────────────────────────────────────────────────────────┐         │
│         │                                                          │         │
│         │                       🍴  Smart POS                      │         │
│         │                       Welcome back                       │         │
│         │                                                          │         │
│         │  Username                                                │         │
│         │  ┌──────────────────────────────────────────────────┐    │         │
│         │  │ nadia                                            │    │         │
│         │  └──────────────────────────────────────────────────┘    │         │
│         │                                                          │         │
│         │  Password                                                │         │
│         │  ┌──────────────────────────────────────────────────┐    │         │
│         │  │ •••••••••                                       👁│    │         │
│         │  └──────────────────────────────────────────────────┘    │         │
│         │                                                          │         │
│         │  [✓] Remember me           Forgot password?              │         │
│         │                                                          │         │
│         │  ┌──────────────────────────────────────────────────┐    │         │
│         │  │                   Sign in                        │    │         │
│         │  └──────────────────────────────────────────────────┘    │         │
│         │                                                          │         │
│         └──────────────────────────────────────────────────────────┘         │
│                                                                              │
│        Tip: default admin is admin / admin123  (change after first run)      │
└──────────────────────────────────────────────────────────────────────────────┘
```

- Password field uses `ImGuiInputTextFlags_Password`; eye 👁 toggles visibility.
- Invalid credentials → red toast top-right + per-frame shake animation on the box.

---

## 3. Dashboard

```
┌────────────┬─────────────────────────────────────────────────────────────────┐
│ ...sidebar │ Dashboard                                            Today      │
├────────────┤                                                                 │
│            │ ┌──── $1,284.50 ────┐  ┌──── 47 ────┐  ┌──── 12 ────┐           │
│            │ │  Today's Revenue  │  │ Active Ord.│  │ Low Stock  │           │
│            │ │  ▲ +12% vs Yest.  │  │  pending: 8│  │  items     │           │
│            │ └───────────────────┘  └────────────┘  └────────────┘           │
│            │                                                                 │
│            │ ┌─── Top Selling ───┐  ┌─── Revenue (last 7 days) ──────────┐   │
│            │ │ 1. Cheeseburger 42│  │     ▁▂▄▇█▅▆                         │📈│
│            │ │ 2. Iced Latte   31│  │  Mon Tue Wed Thu Fri Sat Sun         │   │
│            │ │ 3. BBQ Wings    27│  │  hand-drawn LineChart via ImDrawList │   │
│            │ │ 4. Lava Cake    18│  └──────────────────────────────────────┘   │
│            │ │ 5. Sweet&Sour   16│                                            │
│            │ └───────────────────┘                                            │
│            │                                                                 │
│            │ ┌──── Low Stock Alerts ───────────────────────────────────────┐ │
│            │ │  ⚠  Lettuce      380 g  (threshold 400)   [Restock]         │ │
│            │ │  ⚠  Coffee Beans 220 g  (threshold 400)   [Restock]         │ │
│            │ │  ⚠  Burger Bun    12 pcs (threshold 15)   [Restock]         │ │
│            │ └─────────────────────────────────────────────────────────────┘ │
└────────────┴─────────────────────────────────────────────────────────────────┘
```

- Cards built with `ui::widgets::Card`.
- Revenue line is a manually drawn `LineChart` (`ImDrawList::AddLine`/`AddRectFilled`).
- "Restock" buttons jump to `InventoryScreen` pre-filtered to that ingredient.

---

## 4. Menu Management

```
┌────────────┬─────────────────────────────────────────────────────────────────┐
│ ...sidebar │ Menu Management                            [ + Add New Item ]   │
├────────────┤                                                                 │
│            │ ( All ) ( Fast Food ) ( BBQ ) ( Chinese ) ( Drinks ) ( Desserts)│
│            │ 🔍 [ search items by name...                                  ] │
│            │                                                                 │
│            │ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐ ┌────────┐          │
│            │ │ 🍔 IMG │ │ 🍗 IMG │ │ 🥡 IMG │ │ ☕ IMG │ │ 🍰 IMG │          │
│            │ │        │ │        │ │        │ │        │ │ ⛔ off │          │
│            │ │Cheese  │ │BBQ     │ │Sweet & │ │Iced    │ │Choc    │          │
│            │ │burger  │ │Chicken │ │Sour    │ │Latte   │ │Lava    │          │
│            │ │ $7.99  │ │$12.99  │ │$10.99  │ │ $4.49  │ │ $5.99  │          │
│            │ │ [Edit] │ │ [Edit] │ │ [Edit] │ │ [Edit] │ │ [Edit] │          │
│            │ │ [Del]  │ │ [Del]  │ │ [Del]  │ │ [Del]  │ │ [Del]  │          │
│            │ └────────┘ └────────┘ └────────┘ └────────┘ └────────┘          │
│            │                                                                 │
│            │ ┌────────┐ ┌────────┐ ...                                       │
│            │                                                                 │
└────────────┴─────────────────────────────────────────────────────────────────┘
```

- Card images come from `assets/img/menu/<imagePath>` via `TextureCache`.
- "Add New Item" opens a `Modal` widget with fields (name, category dropdown, price,
  image path picker, available toggle).
- Cashier sees the same screen with `Edit/Del/Add` hidden — controlled by capability.

---

## 5. Order (Cashier)

```
┌────────────┬─────────────────────────────────────────────────────────────────┐
│ ...sidebar │ New Order   #00124      Cashier: nadia   Table: ▾  Customer: ▾  │
├────────────┤                                                                 │
│            │ ( Fast Food ) ( BBQ ) ( Chinese ) ( Drinks ) ( Desserts )       │
│            │ 🔍 [ filter ]                              ┌─── Cart ───────┐   │
│            │ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐        │ 2× Cheeseburger│   │
│            │ │ 🍔   │ │ 🍟   │ │ 🌭   │ │ 🍕   │        │           15.98│   │
│            │ │Cheese│ │Fries │ │Hot   │ │Mini  │        │ 1× Iced Latte  │   │
│            │ │burger│ │      │ │dog   │ │Pizza │        │            4.49│   │
│            │ │$7.99 │ │$3.49 │ │$4.99 │ │$8.99 │        │ 1× Lava Cake   │   │
│            │ │ [+]  │ │ [+]  │ │ [+]  │ │ [+]  │        │   note: hot    │   │
│            │ └──────┘ └──────┘ └──────┘ └──────┘        │            5.99│   │
│            │ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐        │----------------│   │
│            │ │ ...  │ │ ...  │ │ ...  │ │ ...  │        │ Subtotal  26.46│   │
│            │ │ [+]  │ │ [+]  │ │ [+]  │ │ [+]  │        │ Disc     -2.00│   │
│            │ └──────┘ └──────┘ └──────┘ └──────┘        │ Tax 10%   +2.45│   │
│            │                                            │ ────────────── │   │
│            │ [ + Add Special Instruction ]              │ TOTAL    26.91 │   │
│            │ [ Apply Discount ]                         │ Est. prep ~14m │   │
│            │                                            │ ┌────────────┐ │   │
│            │                                            │ │ Place Order│ │   │
│            │                                            │ └────────────┘ │   │
│            │                                            └────────────────┘   │
└────────────┴─────────────────────────────────────────────────────────────────┘
```

- Tapping `[+]` adds to the **`OrderDraft`** UI-state, NOT to a domain `Order`. The
  Order is only created when **Place Order** is tapped.
- The customer dropdown searches `CustomerService` by phone substring.
- `Estimated prep` is `Order::estimatedPrepMinutes()` updated each frame from the draft.

---

## 6. Billing / Receipt

```
┌────────────┬─────────────────────────────────────────────────────────────────┐
│ ...sidebar │ Receipt — Order #00123                                          │
├────────────┤                                                                 │
│            │ ┌───── Payment ─────────┐  ┌────── Receipt Preview ──────────┐  │
│            │ │ Method:               │  │ ============================== │  │
│            │ │  (●) Cash             │  │     SMART RESTAURANT POS       │  │
│            │ │  ( ) Card             │  │     123 Main Street, City      │  │
│            │ │  ( ) Online (sim)     │  │ ============================== │  │
│            │ │                       │  │ Receipt #: RCPT-000123         │  │
│            │ │ Tendered:             │  │ Order #: 123                   │  │
│            │ │ ┌───────────────┐     │  │ Date: 2026-05-19 14:32         │  │
│            │ │ │ $ 30.00       │     │  │ Cashier: nadia                 │  │
│            │ │ └───────────────┘     │  │ Customer: Ali Khan             │  │
│            │ │ Change:  $ 3.09       │  │ ------------------------------ │  │
│            │ │                       │  │ 2 Cheeseburger    7.99  15.98  │  │
│            │ │ [✓] Email PDF (sim)   │  │ 1 Iced Latte      4.49   4.49  │  │
│            │ │                       │  │ 1 Lava Cake       5.99   5.99  │  │
│            │ │ ┌─────────────────┐   │  │ ------------------------------ │  │
│            │ │ │ Charge & Save   │   │  │ Subtotal              26.46    │  │
│            │ │ └─────────────────┘   │  │ Discount              -2.00    │  │
│            │ │                       │  │ Tax 10.00%             2.45    │  │
│            │ │ [Print]   [Save .txt] │  │ TOTAL                 26.91    │  │
│            │ │ [Export PDF-style]    │  │ Payment: CASH                  │  │
│            │ │                       │  │ Thank you and come again!      │  │
│            │ └───────────────────────┘  └─────────────────────────────────┘  │
└────────────┴─────────────────────────────────────────────────────────────────┘
```

- The preview is a fixed-width monospace text block rendered with `ImGui::TextUnformatted`.
- "Print" writes the same content to a file `data/receipts/RCPT-000123.txt` (no real
  printer driver — clearly labelled).
- The PDF export writes the same text to a `.pdf.txt` neighbor file. Real PDF generation
  is out of scope.

---

## 7. Inventory

```
┌────────────┬─────────────────────────────────────────────────────────────────┐
│ ...sidebar │ Inventory       [ + Add Ingredient ]    [ Recipe Editor ]       │
├────────────┤                                                                 │
│            │ 🔍 [ search ingredient...               ]    Showing: All / Low │
│            │                                                                 │
│            │ ┌────────────────────────────────────────────────────────────┐  │
│            │ │ # | Name           | Unit | Stock | Threshold | Cost | ⚙  │  │
│            │ ├────────────────────────────────────────────────────────────┤  │
│            │ │ 1 | Beef Patty     | pcs  |   48  |    10     | 2.50 |[Edit]│  │
│            │ │ 2 | Burger Bun     | pcs  |   12  | ⚠  15     | 0.45 |[Edit]│  │
│            │ │ 3 | Cheese Slice   | pcs  |   72  |    20     | 0.30 |[Edit]│  │
│            │ │ 4 | Lettuce        | g    |  380  | ⚠ 400     | 0.02 |[Edit]│  │
│            │ │ 5 | Espresso Beans | g    | 1500  |   400     | 0.08 |[Edit]│  │
│            │ │ ...                                                          │  │
│            │ └──────────────────────────────────────────────────────────────┘  │
│            │                                                                 │
│            │ ┌──── Usage (last 7 days) ─────────────────────────────────┐    │
│            │ │  Beef Patty   ████████ 32 pcs                            │📊  │
│            │ │  Burger Bun   ███████ 28 pcs                             │    │
│            │ │  Lettuce      ██████ 220 g                               │    │
│            │ │   hand-drawn BarChart via ImDrawList                     │    │
│            │ └──────────────────────────────────────────────────────────┘    │
└────────────┴─────────────────────────────────────────────────────────────────┘
```

- Low-stock rows show ⚠ + a soft red tint (per-frame color animation between two values).
- "Recipe Editor" opens a modal mapping a `MenuItem` to ingredients with qty inputs.

---

## 8. Tables

```
┌────────────┬─────────────────────────────────────────────────────────────────┐
│ ...sidebar │ Tables       Legend:  🟢 Free   🟠 Occupied   🔵 Reserved        │
├────────────┤                                                                 │
│            │ ┌────────────────────────────────────────────────────────────┐  │
│            │ │                                                            │  │
│            │ │   ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐         │  │
│            │ │   │ T1   │  │ T2   │  │ T3   │  │ T4   │  │ T5   │         │  │
│            │ │   │ 🟢 2 │  │ 🟠 4 │  │ 🔵 4 │  │ 🟢 6 │  │ 🟢 2 │         │  │
│            │ │   └──────┘  └──────┘  └──────┘  └──────┘  └──────┘         │  │
│            │ │                                                            │  │
│            │ │   ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐         │  │
│            │ │   │ T6   │  │ T7   │  │ T8   │  │ T9   │  │ T10  │         │  │
│            │ │   │ 🟢 4 │  │ 🟠 2 │  │ 🟢 8 │  │ 🟢 4 │  │ 🟢 4 │         │  │
│            │ │   └──────┘  └──────┘  └──────┘  └──────┘  └──────┘         │  │
│            │ │                                                            │  │
│            │ └────────────────────────────────────────────────────────────┘  │
│            │                                                                 │
│            │   Tap a table to:  [Reserve]   [Mark Occupied]   [Free]         │
│            │   Reserved for: Ali Khan (+92 300 1234567)    14:30             │
└────────────┴─────────────────────────────────────────────────────────────────┘
```

- Tap-to-select shows a popup with the three actions appropriate to that table's
  status. Reserve opens a small "phone-search-or-create-customer" modal.

---

## 9. Kitchen Display

```
┌────────────┬─────────────────────────────────────────────────────────────────┐
│ ...sidebar │ Kitchen Display              ●  Live · ⏰ 14:32:08              │
├────────────┤                                                                 │
│            │ ┌── PENDING (8) ──┐  ┌── PREPARING (3) ─┐  ┌── READY (2) ─────┐ │
│            │ │ #124 · 14:31    │  │ #121 · 14:22 ⏱9m │  │ #119 · 14:14 ⏱18m│ │
│            │ │ 2× Cheeseburger │  │ 1× BBQ Wings     │  │ 1× Sweet & Sour  │ │
│            │ │ 1× Iced Latte   │  │ 1× Iced Latte    │  │ 1× Spring Roll   │ │
│            │ │ 1× Lava Cake    │  │  note: extra hot │  │                  │ │
│            │ │  note: extra hot│  │ [Mark Ready]     │  │ [Mark Served]    │ │
│            │ │ [Start Prep]    │  │                  │  │                  │ │
│            │ ├─────────────────┤  ├──────────────────┤  ├──────────────────┤ │
│            │ │ #125 · 14:32    │  │ #122 · 14:26 ⏱6m │  │ #120 · 14:18 ⏱14m│ │
│            │ │ 1× Cheeseburger │  │ 3× Cheeseburger  │  │ 2× Iced Latte    │ │
│            │ │ [Start Prep]    │  │ [Mark Ready]     │  │ [Mark Served]    │ │
│            │ ├─────────────────┤  ├──────────────────┤  └──────────────────┘ │
│            │ │ ...             │  │ #123 · 14:30 ⏱2m │                      │
│            │ │                 │  │ [Mark Ready]     │                      │
│            │ └─────────────────┘  └──────────────────┘                      │
└────────────┴─────────────────────────────────────────────────────────────────┘
```

- Three vertical lanes, each scrollable.
- Ticket cards show elapsed time (per-frame `DateTime::now() - ticket.queuedAt`).
- Cards animate (slide + fade) when moving between lanes — `Animation::easeOutQuad`.

---

## 10. Analytics

```
┌────────────┬─────────────────────────────────────────────────────────────────┐
│ ...sidebar │ Analytics    Range: [ 2026-05-12 ▾ ] to [ 2026-05-19 ▾ ]        │
├────────────┤              Group: (Day)(Week)(Month)        [ Export CSV ]    │
│            │                                                                 │
│            │ ┌── $7,420.50 ──┐  ┌── 248 orders ──┐  ┌── $29.92 AOV ──┐       │
│            │ │   Revenue     │  │  Tot. Orders   │  │   Avg Order    │       │
│            │ └───────────────┘  └────────────────┘  └────────────────┘       │
│            │                                                                 │
│            │ ┌── Revenue by Day ────────────────────────────────────────┐    │
│            │ │   $                                                      │📊  │
│            │ │   ▇  ▆  █  ▅  ▆  ▇  ▆                                    │    │
│            │ │   Mon Tue Wed Thu Fri Sat Sun                            │    │
│            │ │     hand-drawn BarChart via ImDrawList                   │    │
│            │ └──────────────────────────────────────────────────────────┘    │
│            │                                                                 │
│            │ ┌── Top 5 Items ───────┐  ┌── Sales by Category ─────────────┐  │
│            │ │ 1 Cheeseburger   ###│  │            ⌒                     │📊│
│            │ │ 2 Iced Latte     ## │  │       FastFood 42%               │  │
│            │ │ 3 BBQ Wings      ## │  │         BBQ 21%   ⌒              │  │
│            │ │ 4 Lava Cake      #  │  │   Chinese 16%                    │  │
│            │ │ 5 Sweet & Sour   #  │  │     Drinks 14%                   │  │
│            │ │                     │  │     Desserts 7%                  │  │
│            │ │  small BarChart     │  │   hand-drawn PieChart (ImDrawList)│  │
│            │ └─────────────────────┘  └──────────────────────────────────┘  │
│            │                                                                 │
│            │ ┌── Peak Hours ───────────────────────────────────────────┐    │
│            │ │  0  3  6  9 12 15 18 21                                  │📈  │
│            │ │  ▁  ▁  ▂  ▅  ▇  █  ▆  ▃                                  │    │
│            │ │   hand-drawn LineChart via ImDrawList                    │    │
│            │ └──────────────────────────────────────────────────────────┘    │
│            │                                                                 │
│            │ ┌── Employee Performance ───────────────────────────────────┐   │
│            │ │ # | Cashier | Orders | Revenue   | AvgOrder   | TopItem  │   │
│            │ │ 1 | nadia   |   86   |  $2,840   |  $33.02    | Burger   │   │
│            │ │ 2 | omar    |   71   |  $2,210   |  $31.13    | BBQ      │   │
│            │ │ 3 | iqra    |   91   |  $2,370   |  $26.04    | Latte    │   │
│            │ └─────────────────────────────────────────────────────────┘   │
└────────────┴─────────────────────────────────────────────────────────────────┘
```

- All three chart placements are **manually drawn** with `ImDrawList`:
  - `BarChart` — `AddRectFilled`
  - `PieChart` — `PathArcTo` + `PathFillConvex`
  - `LineChart` — `AddLine` + `AddCircleFilled`
- "Export CSV" writes a flat CSV from the `Report` to `data/reports/`.

---

## 11. Toast / Notification (overlay across screens)

```
                                                    ┌───────────────────────┐
                                                    │ ✅  Order #00124 saved │
                                                    │  Total $26.91         │
                                                    └───────────────────────┘
                                                    ┌───────────────────────┐
                                                    │ ⚠  Lettuce low stock  │
                                                    │  380g / 400g threshold│
                                                    └───────────────────────┘
                                                    ┌───────────────────────┐
                                                    │ ❌  Order failed: stock│
                                                    │  Beef Patty short by 1│
                                                    └───────────────────────┘
```

- Toasts stack top-right; each has a TTL (~4s) and slides out (per-frame easing).
- The `ToastQueue` is a `std::deque<Toast>` in UI state.

---

## 12. Modal pattern (used by Add/Edit/Reserve/Discount)

```
┌──────────── (the whole screen dims behind) ─────────────┐
│                                                          │
│           ┌──────── Edit Menu Item ─────────┐            │
│           │ Name                             │            │
│           │ ┌─────────────────────────────┐  │            │
│           │ │ Classic Cheeseburger        │  │            │
│           │ └─────────────────────────────┘  │            │
│           │ Category   [ FastFood     ▾ ]    │            │
│           │ Price      [ 7.99       ]        │            │
│           │ Image      [ burger.png ] [Pick] │            │
│           │ Available  (●) Yes ( ) No        │            │
│           │                                  │            │
│           │            [Cancel]   [Save]     │            │
│           └──────────────────────────────────┘            │
└──────────────────────────────────────────────────────────┘
```

- `ui::widgets::Modal(id, body)` wraps `ImGui::OpenPopup` + `BeginPopupModal`.
- Background dim is a full-screen rectangle from `GetForegroundDrawList()`.

---

## 13. Theme reference

| Token | Dark | Light |
|---|---|---|
| Window bg | `#0F1115` | `#FAFAFA` |
| Card bg | `#171A21` | `#FFFFFF` |
| Border | `#262B33` | `#E5E7EB` |
| Primary text | `#F3F4F6` | `#111827` |
| Secondary text | `#9CA3AF` | `#6B7280` |
| Accent (Smart POS orange) | `#FF6B35` | `#FF6B35` |
| Success | `#22C55E` | `#16A34A` |
| Warning | `#F59E0B` | `#D97706` |
| Danger | `#EF4444` | `#DC2626` |

`Theme::apply(Mode)` writes these into `ImGuiStyle.Colors[ImGuiCol_*]` and tweaks
`FrameRounding`, `WindowRounding`, etc., to give a soft modern look.

---

*End of `09-gui-wireframes.md`.*
