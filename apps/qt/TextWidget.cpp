#include "TextWidget.h"
#include <QBoxLayout>

TextWidget::TextWidget(QWidget *parent, const QString &text)
    : QWidget(parent)
    , label(new QLabel(text, this))
{
    const auto layout{ new QHBoxLayout{ this } };
    layout->addWidget(label);
}
