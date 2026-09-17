#include <cassert>
#include <string>

#include "ui/widget.h"
#include "ui/theme.h"
#include "ui/ui_manager.h"

int main() {
    using namespace q3d::ui;

    Theme theme;
    UIManager::instance().set_theme(theme);

    auto panel = std::make_shared<Panel>();
    panel->set_id("root");
    panel->show();
    panel->hide();
    panel->set_enabled(true);
    panel->set_enabled(false);

    auto button = std::make_shared<Button>();
    button->set_id("btn");

    auto label = std::make_shared<Label>();
    label->set_id("lbl");

    assert(panel->id() == "root");
    assert(button->id() == "btn");
    assert(label->id() == "lbl");
    assert(UIManager::instance().theme().name.empty());

    return 0;
}