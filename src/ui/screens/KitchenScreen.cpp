#include "ui/screens/Screens.h"
#include "ui/AppContext.h"
#include "ui/state/ToastQueue.h"

#include "domain/auth/Session.h"
#include "domain/common/Exceptions.h"
#include "domain/kitchen/KitchenService.h"
#include "domain/menu/MenuService.h"
#include "domain/order/OrderService.h"

#include "imgui.h"

#include <cstdio>

namespace pos::ui::screens {

static void lane(pos::ui::AppContext& ctx, const char* title,
                 pos::domain::OrderStatus status, const char* advanceLabel,
                 float laneW) {
    using namespace pos::domain;
    auto tickets = ctx.kitchen.lane(status);

    ImGui::BeginChild(title, ImVec2(laneW, -1), true);
    ImGui::Text("%s (%d)", title, (int)tickets.size());
    ImGui::Separator();
    for (const auto& t : tickets) {
        ImGui::PushID((int)t.orderId.value());
        ImGui::BeginChild("##t", ImVec2(-1, 130), true);
        {
            std::int64_t age = DateTime::now().epoch() - t.queuedAt.epoch();
            int mins = (int)(age / 60);
            ImGui::Text("#%u  queued %s  (%dm)", t.orderId.value(),
                        t.queuedAt.format("%H:%M").c_str(), mins);
            ImGui::Separator();
            for (const auto& it : t.items) {
                std::string nm;
                try { nm = ctx.menu.byId(it.menuItem()).name(); }
                catch (...) { nm = "Item #" + std::to_string(it.menuItem().value()); }
                ImGui::Text("%dx %s", it.qty(), nm.c_str());
            }
            if (!t.specialInstructions.empty()) {
                ImGui::TextColored(ImVec4(1, 0.7f, 0.3f, 1), "note: %s",
                                   t.specialInstructions.c_str());
            }
            if (ImGui::SmallButton(advanceLabel)) {
                try {
                    OrderStatus next = OrderStatus::Pending;
                    if (status == OrderStatus::Pending)     next = OrderStatus::Preparing;
                    else if (status == OrderStatus::Preparing) next = OrderStatus::Ready;
                    else if (status == OrderStatus::Ready)     next = OrderStatus::Served;
                    ctx.orders.setStatus(t.orderId, next, ctx.session.user());
                    if (next == OrderStatus::Served) ctx.toasts.success("Served", "Order #" + std::to_string(t.orderId.value()));
                } catch (const DomainException& e) {
                    ctx.toasts.danger("Status update failed", e.what());
                }
            }
        }
        ImGui::EndChild();
        ImGui::PopID();
    }
    ImGui::EndChild();
}

void DrawKitchen(pos::ui::AppContext& ctx) {
    using namespace pos::domain;
    ImGui::Text("Kitchen Display");
    ImGui::SameLine(); ImGui::Dummy(ImVec2(20,0)); ImGui::SameLine();
    ImGui::TextDisabled("Live - tap a ticket button to advance status.");
    ImGui::Separator();

    float laneW = (ImGui::GetContentRegionAvail().x - 20.f) / 3.f;
    lane(ctx, "PENDING",   OrderStatus::Pending,   "Start Prep",  laneW); ImGui::SameLine();
    lane(ctx, "PREPARING", OrderStatus::Preparing, "Mark Ready",  laneW); ImGui::SameLine();
    lane(ctx, "READY",     OrderStatus::Ready,     "Mark Served", laneW);
}

} // namespace pos::ui::screens
