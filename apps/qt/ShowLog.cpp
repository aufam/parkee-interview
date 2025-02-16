#include "ShowLog.h"

#include <QScrollArea>
#include <QBoxLayout>
#include <QGroupBox>
#include <QLabel>

using namespace Qt::Literals::StringLiterals;

ShowLog::ShowLog(const QList<QString>& log, QWidget *parent) : QDialog(parent) {
    setWindowTitle(u"Log info"_s);

    auto scroll = new QScrollArea(this);
    auto widget = new QWidget(this);
    scroll->setWidget(widget);
    scroll->setWidgetResizable(true);

    auto layout = new QVBoxLayout(widget);

    for (const auto &msg : log) {
        auto label = new QLabel(msg, this);
        layout->addWidget(label);
    }

    auto dialogLayout = new QVBoxLayout(this);
    dialogLayout->addWidget(scroll);
}

ShowLog::~ShowLog() {}