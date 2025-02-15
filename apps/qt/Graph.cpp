#include "Graph.h"
#include "ThemeInfo.h"

CustomGraph::CustomGraph(QCPAxis *keyAxis, QCPAxis *valueAxis) : QCPGraph(keyAxis, valueAxis) {
    const qreal lineWidth = 1;
    const qreal plotScatterSize = 10;
    const auto plotSelectionColor = QColor(80, 80, 255);

    setSelectable(QCP::stSingleData);

    setPen(QPen(QBrush(Qt::black), lineWidth));

    setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, Qt::transparent, plotScatterSize));

    selectionDecorator()->setScatterStyle(QCPScatterStyle(
        QCPScatterStyle::ssCross,
        QPen(QBrush(plotSelectionColor), lineWidth + 1),
        QBrush(plotSelectionColor),
        plotScatterSize
    ));

    setPen(QPen(ThemeInfo::GetWindowTextColor()));
}

CustomGraph::~CustomGraph() {}
