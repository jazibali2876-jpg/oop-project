#include "ui/screens/Screens.h"
#include "ui/AppContext.h"
#include "ui/state/ToastQueue.h"
#include "ui/theme/Theme.h"

#include "domain/common/Exceptions.h"
#include "domain/customer/CustomerService.h"
#include "domain/tables/Table.h"
#include "domain/tables/TableService.h"

#include "imgui.h"

#include <cstring>

namespace pos::ui::screens {

namespace {
struct ReserveState {
    bool open = false;
    pos::domain::TableId id{};
    char phone[24] = {};
    char name[40]  = {};
};
ReserveState gReserve;
}

void DrawTables(pos::ui::AppContext& ctx) {
    using namespace pos::domain;

    ImGui::Text("Tables");
    ImGui::SameLine(); ImGui::Dummy(ImVec2(20,0)); ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(pos::ui::Theme::colorSuccess));
    ImGui::Text("Free");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(pos::ui::Theme::colorWarning));
    ImGui::Text("Occupied");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::ColorConvertU32ToFloat4(pos::ui::Theme::colorInfo));
    ImGui::Text("Reserved");
    ImGui::PopStyleColor();
    ImGui::Separator();

    const auto& tables = ctx.tables.all();
    const float cellW = 130.f, cellH = 90.f;
    int colsPerRow = std::max(1, (int)(ImGui::GetContentRegionAvail().x / (cellW + 10)));
    int idx = 0;
    for (const auto& t : tables) {
        if (idx % colsPerRow != 0) ImGui::SameLine();
        ImU32 col;
        switch (t.status()) {
            case TableStatus::Occupied: col = pos::ui::Theme::colorWarning; break;
            case TableStatus::Reserved: col = pos::ui::Theme::colorInfo;    break;
            default:                    col = pos::ui::Theme::colorSuccess; break;
        }
        ImGui::PushID((int)t.id().value());
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::ColorConvertU32ToFloat4(col));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
            ImGui::ColorConvertU32ToFloat4(col & 0xFFFFFFE0));
        char lbl[64];
        std::snprintf(lbl, sizeof(lbl), "T%u\n%d seats\n%s",
                      t.id().value(), t.seats(),
                      tableStatusName(t.status()).c_str());
        if (ImGui::Button(lbl, ImVec2(cellW, cellH))) ImGui::OpenPopup("##actions");
        ImGui::PopStyleColor(2);

        if (ImGui::BeginPopup("##actions")) {
            if (ImGui::MenuItem("Reserve")) {
                gReserve.open = true; gReserve.id = t.id();
                std::memset(gReserve.phone, 0, sizeof(gReserve.phone));
                std::memset(gReserve.name,  0, sizeof(gReserve.name));
            }
            if (ImGui::MenuItem("Mark Occupied")) {
                try { ctx.tables.occupy(t.id()); ctx.toasts.info("Occupied", "Table " + std::to_string(t.id().value())); }
                catch (const DomainException& e) { ctx.toasts.danger("Failed", e.what()); }
            }
            if (ImGui::MenuItem("Free")) {
                try { ctx.tables.free(t.id()); ctx.toasts.info("Freed", "Table " + std::to_string(t.id().value())); }
                catch (const DomainException& e) { ctx.toasts.danger("Failed", e.what()); }
            }
            ImGui::EndPopup();
        }
        ImGui::PopID();
        ++idx;
    }

    if (gReserve.open) { ImGui::OpenPopup("Reserve Table"); gReserve.open = false; }
    ImGui::SetNextWindowSize(ImVec2(380, 0));
    if (ImGui::BeginPopupModal("Reserve Table", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Reserving table T%u", gReserve.id.value());
        ImGui::InputText("Phone", gReserve.phone, sizeof(gReserve.phone));
        ImGui::InputText("Name",  gReserve.name,  sizeof(gReserve.name));
        ImGui::Separator();
        if (ImGui::Button("Cancel", ImVec2(150, 0))) ImGui::CloseCurrentPopup();
        ImGui::SameLine();
        if (ImGui::Button("Reserve", ImVec2(150, 0))) {
            try {
                if (gReserve.phone[0] == '\0') throw DomainException("phone required");
                auto c = ctx.customers.findByPhoneOrCreate(gReserve.phone, gReserve.name[0] ? gReserve.name : "Walk-in");
                ctx.tables.reserve(gReserve.id, c.id());
                ctx.toasts.success("Reserved", "Table " + std::to_string(gReserve.id.value()) + " for " + c.name());
                ImGui::CloseCurrentPopup();
            } catch (const DomainException& e) {
                ctx.toasts.danger("Reserve failed", e.what());
            }
        }
        ImGui::EndPopup();
    }
}

} // namespace pos::ui::screens
