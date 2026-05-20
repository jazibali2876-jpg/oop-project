#pragma once

namespace pos::domain {

enum class Capability {
    ViewMenu,
    EditMenu,
    EditInventory,
    PlaceOrder,
    ManageTables,
    ManageCustomers,
    ViewKitchen,
    AdvanceTicket,
    ViewAnalytics,
    ManageUsers
};

} // namespace pos::domain
