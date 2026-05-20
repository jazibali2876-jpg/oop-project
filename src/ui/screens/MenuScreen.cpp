#include "ui/screens/Screens.h"
#include "ui/AppContext.h"
#include "ui/state/ToastQueue.h"

#include "domain/auth/Session.h"
#include "domain/auth/User.h"
#include "domain/common/Capability.h"
#include "domain/common/Exceptions.h"
#include "domain/menu/MenuService.h"

#include "imgui.h"

#include <cstring>

namespace pos::ui::screens {

namespace {
struct EditState {
    bool open = false;
    bool isNew = false;
    pos::domain::MenuItemId id{};
    char  name[64] = {};
    int   category = 0;
    float priceDollars = 0.f;
    char  imagePath[64] = {};
    bool  available = true;
};
EditState gEdit;

const char* kCategories[] = {"FastFood", "BBQ", "Chinese", "Drinks", "Desserts"};
} // namespace

static void openEditor(const pos::domain::MenuItem* existing, pos::ui::AppContext& ctx) {
    gEdit.open = true;
    if (existing) {
        gEdit.isNew = false;
        gEdit.id = existing->id();
        std::strncpy(gEdit.name, existing->name().c_str(), sizeof(gEdit.name) - 1);
        gEdit.category = (int)existing->category();
        gEdit.priceDollars = (float)existing->price().toDouble();
        std::strncpy(gEdit.imagePath, existing->imagePath().c_str(), sizeof(gEdit.imagePath) - 1);
        gEdit.available = existing->available();
    } else {
        gEdit.isNew = true;
        gEdit.id = pos::domain::MenuItemId{ctx.menu.nextId()};
        std::memset(gEdit.name, 0, sizeof(gEdit.name));
        gEdit.category = 0;
        gEdit.priceDollars = 0.f;
        std::memset(gEdit.imagePath, 0, sizeof(gEdit.imagePath));
        gEdit.available = true;
    }
}

void DrawMenu(pos::ui::AppContext& ctx) {
    using namespace pos::domain;
    bool canEdit = ctx.session.user().can(Capability::EditMenu);

    static char  searchBuf[64] = {};
    static int   catFilter = -1;

    ImGui::Text("Menu");
    ImGui::SameLine(ImGui::GetContentRegionAvail().x - 160);
    if (canEdit) {
        if (ImGui::Button("+ Add New Item", ImVec2(160, 30))) openEditor(nullptr, ctx);
    }

    ImGui::Spacing();
    if (ImGui::RadioButton("All", catFilter == -1)) catFilter = -1;
    for (int i = 0; i < 5; ++i) {
        ImGui::SameLine();
        if (ImGui::RadioButton(kCategories[i], catFilter == i)) catFilter = i;
    }
    ImGui::SameLine(); ImGui::Dummy(ImVec2(20,0)); ImGui::SameLine();
    ImGui::PushItemWidth(260);
    ImGui::InputTextWithHint("##search", "Search items...", searchBuf, sizeof(searchBuf));
    ImGui::PopItemWidth();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    auto results = ctx.menu.filter(
        catFilter < 0 ? std::optional<Category>{} : std::optional<Category>(static_cast<Category>(catFilter)),
        searchBuf);

    ImGui::BeginChild("##grid", ImVec2(-1, -1), false);
    const float colW = 200.f;
    const float colH = 170.f;
    int colsPerRow = std::max(1, (int)(ImGui::GetContentRegionAvail().x / (colW + 10)));
    int idx = 0;
    for (const auto& m : results) {
        if (idx % colsPerRow != 0) ImGui::SameLine();
        ImGui::PushID(idx);
        ImGui::BeginChild("##card", ImVec2(colW, colH), true);
        {
            ImGui::Text("%s", m.name().c_str());
            ImGui::TextDisabled("%s", categoryName(m.category()).c_str());
            ImGui::Text("%s", m.price().format().c_str());
            if (!m.available()) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0.4f, 0.4f, 1));
                ImGui::Text("Unavailable");
                ImGui::PopStyleColor();
            }
            float btnY = colH - 40.f;
            ImGui::SetCursorPosY(btnY);
            if (canEdit) {
                if (ImGui::Button("Edit", ImVec2(colW * 0.45f, 28))) openEditor(&m, ctx);
                ImGui::SameLine();
                if (ImGui::Button("Delete", ImVec2(colW * 0.45f, 28))) {
                    try { ctx.menu.remove(m.id()); ctx.toasts.success("Deleted", m.name()); }
                    catch (const DomainException& e) { ctx.toasts.danger("Delete failed", e.what()); }
                }
            }
        }
        ImGui::EndChild();
        ImGui::PopID();
        ++idx;
    }
    ImGui::EndChild();

    // Editor modal
    if (gEdit.open) {
        ImGui::OpenPopup(gEdit.isNew ? "Add Menu Item" : "Edit Menu Item");
        gEdit.open = false; // ImGui retains popup state via this id
    }
    ImGui::SetNextWindowSize(ImVec2(420, 0));
    if (ImGui::BeginPopupModal(gEdit.isNew ? "Add Menu Item" : "Edit Menu Item",
                               nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextDisabled("ID: %u", gEdit.id.value());
        ImGui::InputText("Name", gEdit.name, sizeof(gEdit.name));
        ImGui::Combo("Category", &gEdit.category, kCategories, IM_ARRAYSIZE(kCategories));
        ImGui::InputFloat("Price ($)", &gEdit.priceDollars, 0.25f, 1.0f, "%.2f");
        ImGui::InputText("Image file", gEdit.imagePath, sizeof(gEdit.imagePath));
        ImGui::Checkbox("Available", &gEdit.available);
        ImGui::Separator();
        if (ImGui::Button("Cancel", ImVec2(150, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::SameLine();
        if (ImGui::Button("Save", ImVec2(150, 0))) {
            try {
                MenuItem mi(gEdit.id, gEdit.name,
                            static_cast<Category>(gEdit.category),
                            Money::fromDouble(gEdit.priceDollars),
                            gEdit.imagePath,
                            gEdit.available);
                if (gEdit.isNew) ctx.menu.add(mi);
                else             ctx.menu.update(mi);
                ctx.toasts.success("Saved", mi.name());
                ImGui::CloseCurrentPopup();
            } catch (const DomainException& e) {
                ctx.toasts.danger("Save failed", e.what());
            }
        }
        ImGui::EndPopup();
    }
}

} // namespace pos::ui::screens
