#include "ui/screens/Screens.h"
#include "ui/AppContext.h"
#include "ui/state/ToastQueue.h"

#include "domain/auth/Session.h"
#include "domain/billing/BillingService.h"
#include "domain/billing/PdfExporter.h"
#include "domain/billing/Receipt.h"
#include "domain/common/Exceptions.h"
#include "domain/order/Order.h"
#include "domain/order/OrderRepository.h"
#include "domain/order/OrderService.h"

#include "imgui.h"

#include <optional>
#include <sstream>

namespace pos::ui::screens {

void DrawBilling(pos::ui::AppContext& ctx) {
    using namespace pos::domain;

    static int  paymentChoice = 0;             // 0=Cash 1=Card 2=Online
    static int  selectedOrderId = 0;

    // Pick the most recent un-served order to default to.
    auto active = ctx.orders.active();
    if (active.empty()) {
        ImGui::TextDisabled("No active orders. Place an order from the Order screen.");
        return;
    }
    if (selectedOrderId == 0 || !ctx.orderRepo.byId(OrderId{(std::uint32_t)selectedOrderId})) {
        selectedOrderId = (int)active.back().id().value();
    }

    // LEFT: payment controls
    float leftW = 320.f;
    ImGui::BeginChild("##pay", ImVec2(leftW, -1), true);
    {
        ImGui::Text("Payment");
        ImGui::Separator();

        ImGui::TextDisabled("Active orders");
        for (const auto& o : active) {
            char lbl[64];
            std::snprintf(lbl, sizeof(lbl), "#%u  %s  %s",
                          o.id().value(),
                          orderStatusName(o.status()).c_str(),
                          o.total().format().c_str());
            if (ImGui::Selectable(lbl, selectedOrderId == (int)o.id().value())) {
                selectedOrderId = (int)o.id().value();
            }
        }
        ImGui::Separator();

        ImGui::Text("Method");
        ImGui::RadioButton("Cash",       &paymentChoice, 0);
        ImGui::RadioButton("Card",       &paymentChoice, 1);
        ImGui::RadioButton("Online sim", &paymentChoice, 2);

        ImGui::Separator();
        auto order = ctx.orderRepo.byId(OrderId{(std::uint32_t)selectedOrderId});
        if (!order) { ImGui::TextDisabled("Order not found."); ImGui::EndChild(); return; }

        if (ImGui::Button("Charge & Save Receipt", ImVec2(-1, 40))) {
            try {
                PaymentMethod m = static_cast<PaymentMethod>(paymentChoice);
                Order o = *order;
                Receipt r = ctx.billing.recordPayment(o, m);
                std::string path = ctx.billing.saveReceiptFile(r);
                ctx.orders.setStatus(o.id(), OrderStatus::Served, ctx.session.user());
                ctx.toasts.success("Receipt saved", path);
            } catch (const DomainException& e) {
                ctx.toasts.danger("Charge failed", e.what());
            }
        }

        if (ImGui::Button("Export PDF-style", ImVec2(-1, 28))) {
            try {
                Order o = *order;
                Receipt r = ctx.billing.makeReceipt(o);
                r.setMethod(static_cast<PaymentMethod>(paymentChoice));
                auto path = PdfExporter::exportReceipt(r);
                ctx.toasts.info("Exported", path);
            } catch (const DomainException& e) {
                ctx.toasts.danger("Export failed", e.what());
            }
        }
    }
    ImGui::EndChild();
    ImGui::SameLine();

    // RIGHT: receipt preview (monospace text)
    ImGui::BeginChild("##receipt", ImVec2(-1, -1), true);
    {
        ImGui::Text("Receipt Preview");
        ImGui::Separator();
        auto order = ctx.orderRepo.byId(OrderId{(std::uint32_t)selectedOrderId});
        if (order) {
            Receipt r = ctx.billing.makeReceipt(*order);
            r.setMethod(static_cast<PaymentMethod>(paymentChoice));
            std::ostringstream oss;
            oss << r;
            ImGui::TextUnformatted(oss.str().c_str());
        }
    }
    ImGui::EndChild();
}

} // namespace pos::ui::screens
