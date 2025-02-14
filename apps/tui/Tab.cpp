#include <ftxui/component/component.hpp>

namespace tui = ftxui;

// build a tab
auto Tab(const std::vector<std::string>& titles, int& index, tui::Components contents) -> tui::Component {
    auto tab_toggle = tui::Menu(&titles, &index, tui::MenuOption::HorizontalAnimated());
    auto tab_container = tui::Container::Tab(std::move(contents), &index);

    return tui::Container::Vertical({
        tab_toggle,
        tab_container,
    });
}
