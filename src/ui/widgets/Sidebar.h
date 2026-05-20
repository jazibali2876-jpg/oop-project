#pragma once

namespace pos::ui {
    class AppContext;
}

namespace pos::ui::widgets {

// Draws the left navigation sidebar. Reads session capabilities to hide entries.
// width is in pixels; pass the full screen height for the sidebar.
void Sidebar(pos::ui::AppContext& ctx, float width, float height);

} // namespace pos::ui::widgets
