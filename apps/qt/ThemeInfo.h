#ifndef THEMEINFO_H
#define THEMEINFO_H

#include <QPalette>

/// theme info, empty class
class ThemeInfo {
public:
    static auto GetWindowColor() {
        return QPalette().color(QPalette::Window);
    }

    static auto GetWindowTextColor() {
        return QPalette().color(QPalette::WindowText);
    }

    static bool IsDarkTheme() {
        const QPalette p;
        return p.color(QPalette::WindowText).lightness() > p.color(QPalette::Window).lightness();
    }
};

#endif
