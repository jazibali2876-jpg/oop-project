#pragma once

namespace pos::domain {
    class AuthService;
    class MenuService;
    class OrderService;
    class BillingService;
    class InventoryService;
    class CustomerService;
    class TableService;
    class KitchenService;
    class AnalyticsService;
    class OrderRepository;
    class SettingsRepository;
    class Session;
}

namespace pos::platform { class TextureCache; }

namespace pos::ui {

class Router;
class ToastQueue;
class OrderDraft;

// Bundle of references passed by reference into every screen draw function.
// Holds NO state — screens read services + push toasts + navigate via router.
struct AppContext {
    pos::domain::AuthService&        auth;
    pos::domain::MenuService&        menu;
    pos::domain::OrderService&       orders;
    pos::domain::BillingService&     billing;
    pos::domain::InventoryService&   inventory;
    pos::domain::CustomerService&    customers;
    pos::domain::TableService&       tables;
    pos::domain::KitchenService&     kitchen;
    pos::domain::AnalyticsService&   analytics;
    pos::domain::OrderRepository&    orderRepo;
    pos::domain::SettingsRepository& settings;
    pos::domain::Session&            session;

    Router&        router;
    ToastQueue&    toasts;
    OrderDraft&    orderDraft;
    float          screenW = 1280.f;
    float          screenH = 800.f;
    float          deltaSeconds = 1.f / 60.f;
    bool           darkTheme = true;
};

} // namespace pos::ui
