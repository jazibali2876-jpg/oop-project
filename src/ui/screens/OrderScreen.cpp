#include "ui/screens/Screens.h"
#include "ui/AppContext.h"
#include "ui/Router.h"
#include "ui/state/OrderDraft.h"
#include "ui/state/ToastQueue.h"

#include "domain/auth/Session.h"
#include "domain/common/Exceptions.h"
#include "domain/customer/CustomerService.h"
#include "domain/menu/MenuService.h"
#include "domain/order/Order.h"
#include "domain/order/OrderService.h"

#include "imgui.h"

#include <cstring>
#include <optional>

namespace pos::ui::screens {

namespace {
const char* kCategories[] = {"FastFood", "BBQ", "Chinese", "Drinks", "Desserts"};
}

void DrawOrder(pos::ui::AppContext& ctx) {
    using namespace pos::domain;

    static int catFilter = -1;
    static char searchBuf[64] = {};
    static char phoneBuf[24] = {};
    static char custNameBuf[40] = {};
    static char specialBuf[64] = {};
    static float discountDollars = 0.f;
    static std::optional<Customer> matchedCustomer;

    float leftW = ImGui::GetContentRegionAvail().x - 360.f;
    if (leftW < 300.f) leftW = ImGui::GetContentRegionAvail().x * 0.65f;

    // LEFT: menu grid + filter
    ImGui::BeginChild("##menu", ImVec2(leftW, -1), false);
    {
        ImGui::Text("Browse Menu");
        ImGui::SameLine(ImGui::GetContentRegionAvail().x - 240);
        ImGui::PushItemWidth(240);
        ImGui::InputTextWithHint("##search", "Search...", searchBuf, sizeof(searchBuf));
        ImGui::PopItemWidth();

        if (ImGui::RadioButton("All", catFilter == -1)) catFilter = -1;
        for (int i = 0; i < 5; ++i) {
            ImGui::SameLine();
            if (ImGui::RadioButton(kCategories[i], catFilter == i)) catFilter = i;
        }
        ImGui::Separator();

        auto items = ctx.menu.filter(
            catFilter < 0 ? std::optional<Category>{} : std::optional<Category>(static_cast<Category>(catFilter)),
            searchBuf);

        const float colW = 170.f;
        const float colH = 130.f;
        int colsPerRow = std::max(1, (int)(ImGui::GetContentRegionAvail().x / (colW + 10)));
        int idx = 0;
        for (const auto& m : items) {
            if (!m.available()) continue;
            if (idx % colsPerRow != 0) ImGui::SameLine();
            ImGui::PushID((int)m.id().value());
            ImGui::BeginChild("##c", ImVec2(colW, colH), true);
            {
                ImGui::TextWrapped("%s", m.name().c_str());
                ImGui::TextDisabled("%s", categoryName(m.category()).c_str());
                ImGui::Text("%s", m.price().format().c_str());
                ImGui::SetCursorPosY(colH - 36);
                if (ImGui::Button("Add to cart", ImVec2(-1, 28))) {
                    ctx.orderDraft.addItem(m.id(), m.price(), m.name());
                }
            }
            ImGui::EndChild();
            ImGui::PopID();
            ++idx;
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // RIGHT: cart + checkout
    ImGui::BeginChild("##cart", ImVec2(-1, -1), true);
    {
        ImGui::Text("Cart");
        ImGui::Separator();

        // Customer block
        ImGui::TextDisabled("Customer (optional)");
        ImGui::PushItemWidth(140);
        ImGui::InputTextWithHint("##phone", "phone", phoneBuf, sizeof(phoneBuf));
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Lookup", ImVec2(80, 22))) {
            if (auto c = ctx.customers.findByPhone(phoneBuf)) {
                matchedCustomer = *c;
                std::strncpy(custNameBuf, c->name().c_str(), sizeof(custNameBuf) - 1);
                ctx.orderDraft.setCustomer(c->id());
                ctx.toasts.info("Found customer", c->name() + " (" + c->tier() + ")");
            } else {
                matchedCustomer.reset();
                std::memset(custNameBuf, 0, sizeof(custNameBuf));
                ctx.orderDraft.setCustomer(std::nullopt);
            }
        }
        if (!matchedCustomer) {
            ImGui::PushItemWidth(-1);
            ImGui::InputTextWithHint("##cname", "Name (for new customer)", custNameBuf, sizeof(custNameBuf));
            ImGui::PopItemWidth();
            if (phoneBuf[0] && custNameBuf[0]) {
                if (ImGui::SmallButton("Register & attach")) {
                    auto c = ctx.customers.findByPhoneOrCreate(phoneBuf, custNameBuf);
                    matchedCustomer = c;
                    ctx.orderDraft.setCustomer(c.id());
                    ctx.toasts.success("Registered", c.name());
                }
            }
        } else {
            ImGui::Text("Attached: %s (%s)", matchedCustomer->name().c_str(),
                        matchedCustomer->tier().c_str());
            if (ImGui::SmallButton("Detach")) {
                matchedCustomer.reset();
                ctx.orderDraft.setCustomer(std::nullopt);
                phoneBuf[0] = '\0'; custNameBuf[0] = '\0';
            }
        }
        ImGui::Separator();

        // Item rows
        const auto& items = ctx.orderDraft.items();
        const auto& names = ctx.orderDraft.names();
        if (items.empty()) {
            ImGui::TextDisabled("Cart is empty. Tap items on the left.");
        } else {
            if (ImGui::BeginTable("cart", 4, ImGuiTableFlags_BordersInnerH)) {
                ImGui::TableSetupColumn("Item",  ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Qty",   ImGuiTableColumnFlags_WidthFixed, 86.f);
                ImGui::TableSetupColumn("Line",  ImGuiTableColumnFlags_WidthFixed, 70.f);
                ImGui::TableSetupColumn("",      ImGuiTableColumnFlags_WidthFixed, 24.f);
                for (std::size_t i = 0; i < items.size(); ++i) {
                    ImGui::TableNextRow();
                    ImGui::PushID((int)i);
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextWrapped("%s",
                        i < names.size() ? names[i].c_str() :
                        ("Item #" + std::to_string(items[i].menuItem().value())).c_str());
                    ImGui::TableSetColumnIndex(1);
                    if (ImGui::SmallButton("-")) ctx.orderDraft.decrement(i);
                    ImGui::SameLine();
                    ImGui::Text("%d", items[i].qty());
                    ImGui::SameLine();
                    if (ImGui::SmallButton("+")) ctx.orderDraft.increment(i);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%s", items[i].lineTotal().format().c_str());
                    ImGui::TableSetColumnIndex(3);
                    if (ImGui::SmallButton("x")) { ctx.orderDraft.removeAt(i); ImGui::PopID(); break; }
                    ImGui::PopID();
                }
                ImGui::EndTable();
            }
        }

        ImGui::Separator();
        ImGui::TextDisabled("Special instructions");
        ImGui::PushItemWidth(-1);
        ImGui::InputText("##special", specialBuf, sizeof(specialBuf));
        ImGui::PopItemWidth();
        ImGui::Text("Discount ($)");
        ImGui::PushItemWidth(-1);
        if (ImGui::InputFloat("##disc", &discountDollars, 0.5f, 1.0f, "%.2f")) {
            if (discountDollars < 0) discountDollars = 0;
            ctx.orderDraft.setDiscount(Money::fromDouble(discountDollars));
        }
        ImGui::PopItemWidth();

        ImGui::Separator();
        ImGui::Text("Subtotal:   %s", ctx.orderDraft.subtotal().format().c_str());
        ImGui::Text("Discount:  -%s", ctx.orderDraft.discount().format().c_str());
        ImGui::Text("Tax (10%%):  %s", ctx.orderDraft.tax().format().c_str());
        ImGui::Separator();
        ImGui::Text("TOTAL:      %s", ctx.orderDraft.total().format().c_str());
        ImGui::TextDisabled("Est. prep: ~%d min", 3 + 2 * ctx.orderDraft.totalQty());

        ImGui::Spacing();
        if (ImGui::Button("Place Order", ImVec2(-1, 40))) {
            try {
                ctx.orderDraft.setSpecialInstructions(specialBuf);
                Order draft = ctx.orderDraft.toOrder();
                Order placed = ctx.orders.place(draft, ctx.session.user(), ctx.orderDraft.customer());
                ctx.toasts.success("Order placed", "Order #" + std::to_string(placed.id().value())
                                                  + "  Total " + placed.total().format());
                ctx.orderDraft.clear();
                std::memset(specialBuf, 0, sizeof(specialBuf));
                std::memset(phoneBuf, 0, sizeof(phoneBuf));
                std::memset(custNameBuf, 0, sizeof(custNameBuf));
                matchedCustomer.reset();
                discountDollars = 0.f;
                ctx.router.go(pos::ui::Screen::Billing);
            } catch (const DomainException& e) {
                ctx.toasts.danger("Order failed", e.what());
            }
        }
        if (ImGui::Button("Clear Cart", ImVec2(-1, 26))) {
            ctx.orderDraft.clear();
            std::memset(specialBuf, 0, sizeof(specialBuf));
            discountDollars = 0;
        }
    }
    ImGui::EndChild();
}

} // namespace pos::ui::screens
