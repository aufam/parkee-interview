#include "Chart.h"

Chart::Chart(QWidget *parent) : QWidget(parent) {
    auto layout = new QHBoxLayout(this);
    layout->addWidget(plot);

    plot->replot();

    timer->setInterval(updateInterval);
    timer->start();

    connect(timer, &QTimer::timeout, this, [this]() {
        if (needsUpdate) {
            needsUpdate = false;
            updateChart();
        }
    });
}

Chart::~Chart() {}

void Chart::updateChart() {
    graph->setData(plotData);

    const bool onlyEnlarge = false;
    const bool inKeyRange = false;
    graph->rescaleKeyAxis(onlyEnlarge);
    graph->rescaleValueAxis(onlyEnlarge, inKeyRange);

    plot->replot();
}

void Chart::addData(const QSharedPointer<const QList<qreal>> receivedData) {
    for (auto value: *receivedData) {
        plotData->add(QCPGraphData{currentPlotDataCount, value});
        currentPlotDataCount += 1.0;
        if (plotData->size() > bufferSize)
            plotData->removeBefore(plotData->constBegin()->key + 0.1);
    }
    needsUpdate = true;
}

void Chart::setUpdateInterval(const int msec) const {
    plot->deselectAll();

    timer->stop();
    timer->setInterval(msec);
    timer->start();
}
