#include "App.h"

#include "platform/Platform.h"
#include "platform/ImGuiBackend.h"
#include "platform/InputBridge.h"

#include "ui/AppContext.h"
#include "ui/Router.h"
#include "ui/Shell.h"
#include "ui/state/OrderDraft.h"
#include "ui/state/ToastQueue.h"
#include "ui/theme/Fonts.h"
#include "ui/theme/Theme.h"

#include "domain/auth/AuthService.h"
#include "domain/auth/Session.h"
#include "domain/auth/UserRepository.h"
#include "domain/customer/CustomerRepository.h"
#include "domain/customer/CustomerService.h"
#include "domain/customer/LoyaltyRepository.h"
#include "domain/menu/MenuRepository.h"
#include "domain/menu/MenuService.h"
#include "domain/inventory/InventoryRepository.h"
#include "domain/inventory/InventoryService.h"
#include "domain/inventory/RecipeRepository.h"
#include "domain/order/OrderRepository.h"
#include "domain/order/OrderService.h"
#include "domain/billing/BillingService.h"
#include "domain/billing/ReceiptRepository.h"
#include "domain/tables/TableRepository.h"
#include "domain/tables/TableService.h"
#include "domain/kitchen/KitchenService.h"
#include "domain/analytics/AnalyticsService.h"
#include "domain/analytics/SalesHistoryRepository.h"
#include "domain/persistence/SettingsRepository.h"
#include "domain/persistence/FilePaths.h"

#include "util/Files.h"
#include "util/Log.h"

#include "imgui.h"

#include <filesystem>

namespace pos {

struct App::Impl {
    // Platform integration
    platform::Platform     platform;
    platform::ImGuiBackend imgui;
    platform::InputBridge  input;

    // Repositories
    domain::UserRepository          userRepo;
    domain::MenuRepository          menuRepo;
    domain::InventoryRepository     invRepo;
    domain::RecipeRepository        recRepo;
    domain::OrderRepository         orderRepo;
    domain::ReceiptRepository       receiptRepo;
    domain::CustomerRepository      custRepo;
    domain::LoyaltyRepository       loyRepo;
    domain::TableRepository         tableRepo;
    domain::SalesHistoryRepository  salesRepo;
    domain::SettingsRepository      settingsRepo;

    // Services
    domain::AuthService       auth      { userRepo };
    domain::MenuService       menu      { menuRepo };
    domain::InventoryService  inventory { invRepo, recRepo };
    domain::CustomerService   customers { custRepo, loyRepo };
    domain::KitchenService    kitchen;
    domain::OrderService      orders    { orderRepo, inventory, kitchen, salesRepo };
    domain::BillingService    billing   { receiptRepo, orderRepo, menu, customers };
    domain::TableService      tables    { tableRepo };
    domain::AnalyticsService  analytics { salesRepo, orderRepo };

    // UI
    domain::Session   session;
    ui::Router        router;
    ui::ToastQueue    toasts;
    ui::OrderDraft    orderDraft;

    bool darkTheme = true;
};

App::App()  : impl_(std::make_unique<Impl>()) {}
App::~App() = default;

static void setWorkingDirToExe() {
    namespace fs = std::filesystem;
    auto exeDir = pos::util::exeDirectory();
    if (!exeDir.empty()) {
        std::error_code ec;
        fs::current_path(exeDir, ec);
    }
}

static std::string locateFontDir() {
    namespace fs = std::filesystem;
    for (fs::path p : {fs::path("assets/fonts"),
                       fs::path("../assets/fonts"),
                       fs::path("../../assets/fonts")}) {
        if (fs::exists(p)) return p.string();
    }
    return "assets/fonts";
}

int App::run() {
    setWorkingDirToExe();
    pos::util::mkdirs(domain::FilePaths::DataDir);
    pos::util::mkdirs(domain::FilePaths::ReceiptsDir);
    pos::util::Log::info("App", "starting Smart Restaurant POS");

    // Settings + theme
    impl_->settingsRepo.load();
    impl_->darkTheme = impl_->settingsRepo.get("theme", "Dark") != "Light";

    // Load all caches and seed defaults on a fresh install.
    impl_->menu.load();        impl_->menu.seedDefaultsIfEmpty();
    impl_->inventory.load();   impl_->inventory.seedDefaultsIfEmpty();
                               impl_->inventory.seedDefaultRecipesIfEmpty();
    impl_->customers.load();
    impl_->tables.load();      impl_->tables.seedDefaultsIfEmpty();
    impl_->auth.seedDefaultAdminIfEmpty();
    impl_->kitchen.rehydrate(impl_->orders.active());

    // Platform: window + ImGui backend
    platform::Config cfg;
    cfg.title  = "Smart Restaurant POS";
    cfg.width  = 1280;
    cfg.height = 800;
    if (!impl_->platform.init(cfg)) {
        pos::util::Log::error("App", "platform init failed");
        return 1;
    }

    std::string fontDir = locateFontDir();
    if (!impl_->imgui.init(impl_->platform.window(), fontDir)) {
        pos::util::Log::error("App", "imgui init failed");
        return 2;
    }
    // Use the proper font atlas (Inter regular/bold + large bold).
    pos::ui::Fonts::load(ImGui::GetIO(), fontDir);
    ImGui::GetIO().Fonts->Build();

    pos::ui::Theme::apply(impl_->darkTheme ? pos::ui::ThemeMode::Dark
                                           : pos::ui::ThemeMode::Light);

    // Main loop
    while (true) {
        if (!impl_->platform.pumpEvents(impl_->input)) break;

        float dt = impl_->platform.deltaSeconds();
        auto fb  = impl_->platform.framebufferSize();
        impl_->imgui.newFrame(dt, fb.x, fb.y);

        pos::ui::AppContext ctx{
            impl_->auth, impl_->menu, impl_->orders, impl_->billing,
            impl_->inventory, impl_->customers, impl_->tables,
            impl_->kitchen, impl_->analytics, impl_->orderRepo, impl_->settingsRepo,
            impl_->session,
            impl_->router, impl_->toasts, impl_->orderDraft,
            (float)fb.x, (float)fb.y, dt, impl_->darkTheme
        };

        pos::ui::Shell::draw(ctx);
        impl_->toasts.tickAndDraw(dt, (float)fb.x);

        impl_->darkTheme = ctx.darkTheme;

        impl_->platform.clear();
        impl_->imgui.render(impl_->platform.window());
        impl_->platform.display();
    }

    pos::util::Log::info("App", "shutting down");
    impl_->imgui.shutdown();
    return 0;
}

} // namespace pos
