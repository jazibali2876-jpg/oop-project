#pragma once

namespace pos::domain {

// All on-disk file paths used by the app. Relative to the executable's current
// working directory (which the App sets to its own folder at startup).
struct FilePaths {
    static constexpr const char* DataDir       = "data";
    static constexpr const char* ReceiptsDir   = "data/receipts";
    static constexpr const char* Reports       = "data/reports";

    static constexpr const char* Users         = "data/users.dat";
    static constexpr const char* Menu          = "data/menu.txt";
    static constexpr const char* Inventory     = "data/inventory.txt";
    static constexpr const char* Recipes       = "data/recipes.txt";
    static constexpr const char* Orders        = "data/orders.dat";
    static constexpr const char* Customers     = "data/customers.dat";
    static constexpr const char* Loyalty       = "data/loyalty.dat";
    static constexpr const char* SalesHistory  = "data/sales_history.dat";
    static constexpr const char* Tables        = "data/tables.txt";
    static constexpr const char* Settings      = "data/settings.txt";
    static constexpr const char* Log           = "data/log.txt";
};

} // namespace pos::domain
