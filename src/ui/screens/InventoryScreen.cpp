#include "ui/screens/Screens.h"
#include "ui/AppContext.h"
#include "ui/state/ToastQueue.h"
#include "ui/theme/Theme.h"

#include "domain/auth/Session.h"
#include "domain/auth/User.h"
#include "domain/common/Capability.h"
#include "domain/common/Exceptions.h"
#include "domain/inventory/InventoryService.h"

#include "imgui.h"

#include <cstring>

namespace pos::ui::screens {

namespace {
struct InvEditState {
    bool open = false;
    bool isNew = false;
    pos::domain::IngredientId id{};
    char  name[40] = {};
    char  unit[12] = {};
    float stock = 0.f;
    float threshold = 0.f;
    float costDollars = 0.f;
};
InvEditState gInvEdit;
}

static void openInvEditor(const pos::domain::Ingredient* existing, pos::ui::AppContext& ctx) {
    gInvEdit.open = true;
    if (existing) {
        gInvEdit.isNew = false;
        gInvEdit.id = existing->id();
        std::strncpy(gInvEdit.name, existing->name().c_str(), sizeof(gInvEdit.name) - 1);
        std::strncpy(gInvEdit.unit, existing->unit().c_str(), sizeof(gInvEdit.unit) - 1);
        gInvEdit.stock = (float)existing->stock();
        gInvEdit.threshold = (float)existing->reorderThreshold();
        gInvEdit.costDollars = (float)existing->costPerUnit().toDouble();
    } else {
        gInvEdit.isNew = true;
        gInvEdit.id = pos::domain::IngredientId{ctx.inventory.nextId()};
        std::memset(gInvEdit.name, 0, sizeof(gInvEdit.name));
        std::strncpy(gInvEdit.unit, "pcs", sizeof(gInvEdit.unit) - 1);
        gInvEdit.stock = 0.f;
        gInvEdit.threshold = 0.f;
        gInvEdit.costDollars = 0.f;
    }
}

void DrawInventory(pos::ui::AppContext& ctx) {
    using namespace pos::domain;

    bool canEdit = ctx.session.user().can(Capability::EditInventory);
    static bool showLowOnly = false;
    static char searchBuf[40] = {};

    ImGui::Text("Inventory");
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 160);
    if (canEdit) {
        if (ImGui::Button("+ Add Ingredient", ImVec2(160, 30))) openInvEditor(nullptr, ctx);
    }

    ImGui::Checkbox("Low stock only", &showLowOnly);
    ImGui::SameLine(); ImGui::Dummy(ImVec2(20,0)); ImGui::SameLine();
    ImGui::PushItemWidth(220);
    ImGui::InputTextWithHint("##search", "search...", searchBuf, sizeof(searchBuf));
    ImGui::PopItemWidth();
    ImGui::Separator();

    if (ImGui::BeginTable("inv", canEdit ? 7 : 6,
                          ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("#");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Unit");
        ImGui::TableSetupColumn("Stock");
        ImGui::TableSetupColumn("Threshold");
        ImGui::TableSetupColumn("Cost / unit");
        if (canEdit) ImGui::TableSetupColumn("Actions");
        ImGui::TableHeadersRow();

        for (const auto& ing : ctx.inventory.all()) {
            if (showLowOnly && !ing.isLow()) continue;
            if (searchBuf[0] && ing.name().find(searchBuf) == std::string::npos) continue;
            ImGui::TableNextRow();
            if (ing.isLow()) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,
                    IM_COL32(245, 158, 11, 35));
            }
            ImGui::TableSetColumnIndex(0); ImGui::Text("%u", ing.id().value());
            ImGui::TableSetColumnIndex(1);
            if (ing.isLow()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(pos::ui::Theme::colorWarning));
                ImGui::Text("LOW  %s", ing.name().c_str());
                ImGui::PopStyleColor();
            } else {
                ImGui::Text("%s", ing.name().c_str());
            }
            ImGui::TableSetColumnIndex(2); ImGui::Text("%s", ing.unit().c_str());
            ImGui::TableSetColumnIndex(3); ImGui::Text("%.1f", ing.stock());
            ImGui::TableSetColumnIndex(4); ImGui::Text("%.1f", ing.reorderThreshold());
            ImGui::TableSetColumnIndex(5); ImGui::Text("%s", ing.costPerUnit().format().c_str());
            if (canEdit) {
                ImGui::TableSetColumnIndex(6);
                ImGui::PushID((int)ing.id().value());
                if (ImGui::SmallButton("Edit")) openInvEditor(&ing, ctx);
                ImGui::SameLine();
                if (ImGui::SmallButton("Delete")) {
                    try { ctx.inventory.removeIngredient(ing.id()); ctx.toasts.success("Removed", ing.name()); }
                    catch (const DomainException& e) { ctx.toasts.danger("Remove failed", e.what()); }
                }
                ImGui::PopID();
            }
        }
        ImGui::EndTable();
    }

    if (gInvEdit.open) { ImGui::OpenPopup(gInvEdit.isNew ? "Add Ingredient" : "Edit Ingredient"); gInvEdit.open = false; }
    ImGui::SetNextWindowSize(ImVec2(420, 0));
    if (ImGui::BeginPopupModal(gInvEdit.isNew ? "Add Ingredient" : "Edit Ingredient",
                               nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextDisabled("ID: %u", gInvEdit.id.value());
        ImGui::InputText("Name", gInvEdit.name, sizeof(gInvEdit.name));
        ImGui::InputText("Unit", gInvEdit.unit, sizeof(gInvEdit.unit));
        ImGui::InputFloat("Stock", &gInvEdit.stock, 1.0f, 10.0f, "%.1f");
        ImGui::InputFloat("Reorder threshold", &gInvEdit.threshold, 1.0f, 10.0f, "%.1f");
        ImGui::InputFloat("Cost per unit ($)", &gInvEdit.costDollars, 0.05f, 0.5f, "%.2f");
        ImGui::Separator();
        if (ImGui::Button("Cancel", ImVec2(150, 0))) ImGui::CloseCurrentPopup();
        ImGui::SameLine();
        if (ImGui::Button("Save", ImVec2(150, 0))) {
            try {
                Ingredient ing(gInvEdit.id, gInvEdit.name, gInvEdit.unit,
                               gInvEdit.stock, gInvEdit.threshold,
                               Money::fromDouble(gInvEdit.costDollars));
                if (gInvEdit.isNew) ctx.inventory.addIngredient(ing);
                else                ctx.inventory.updateIngredient(ing);
                ctx.toasts.success("Saved", ing.name());
                ImGui::CloseCurrentPopup();
            } catch (const DomainException& e) {
                ctx.toasts.danger("Save failed", e.what());
            }
        }
        ImGui::EndPopup();
    }
}

} // namespace pos::ui::screens
