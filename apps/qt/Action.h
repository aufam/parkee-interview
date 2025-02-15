#ifndef ACTION_H
#define ACTION_H

#include <QAction>

/// Action class implementation from QAction
class Action : public QAction {
public:
    using QAction::QAction; //< expose QAction constructor

    /// set shortcut sequence and enable the tooltip
    void setShortcut(const QKeySequence &shortcut);

    /// set icon from icons
    void setIcon(const QString &fileName);
};

#endif
