#include "ui/theme/Fonts.h"

#include <filesystem>

namespace pos::ui {

bool Fonts::load(ImGuiIO& io, const std::string& fontDir) {
    namespace fs = std::filesystem;
    fs::path dir(fontDir);
    fs::path regular = dir / "Inter-Regular.ttf";
    fs::path bold    = dir / "Inter-Bold.ttf";

    io.Fonts->Clear();
    if (fs::exists(regular)) {
        regular_ = io.Fonts->AddFontFromFileTTF(regular.string().c_str(), 17.0f);
    } else {
        regular_ = io.Fonts->AddFontDefault();
    }
    if (fs::exists(bold)) {
        bold_  = io.Fonts->AddFontFromFileTTF(bold.string().c_str(),    17.0f);
        large_ = io.Fonts->AddFontFromFileTTF(bold.string().c_str(),    28.0f);
    } else {
        bold_  = regular_;
        large_ = regular_;
    }
    return regular_ != nullptr;
}

} // namespace pos::ui
