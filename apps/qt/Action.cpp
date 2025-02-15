#include "Action.h"
#include "ThemeInfo.h"
#include <QFileInfo>

using namespace Qt::Literals::StringLiterals;

void Action::setShortcut(const QKeySequence &shortcut) {
    QAction::setShortcut(shortcut);
    setToolTip(toolTip() + u" ("_s + shortcut.toString() + u")"_s);
}

void Action::setIcon(const QString &fileName) {
    auto file = QFileInfo(fileName);
    auto name = file.baseName();
    auto extension = file.suffix();
    auto fileNameWithTheme = name + (ThemeInfo::IsDarkTheme() ? u"White."_s : u"Black."_s) + extension;

    QAction::setIcon(QIcon(fileNameWithTheme));
}
