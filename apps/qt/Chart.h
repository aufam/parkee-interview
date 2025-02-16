#ifndef CHART_H
#define CHART_H

#include "Graph.h"
#include "Plot.h"

#include <QWidget>

/// chart widget implementation
class Chart : public QWidget {
    Q_OBJECT

public:
    static constexpr int DefaultInterval = 20;
    static constexpr ulong DefaultBufferSize = 20;

    explicit Chart(QWidget *parent = nullptr);
    virtual ~Chart() override;

    /// get plot from data container
    const auto &getPlotData() const { return plotData; }

    /// delete all data and update the chart
    void clear() {
        plotData->clear();
        needsUpdate = true;
    }

    void requestUpdate() { needsUpdate = true; }

    void setBufferSize(int value) { bufferSize = value; }
    auto getBufferSize() const { return bufferSize; }

    void setUpdateInterval(int msec) const;
    auto getUpdateInterval() const { return updateInterval; }

private:
    CustomPlot *plot = new CustomPlot(this);
    CustomGraph *graph = new CustomGraph(plot->xAxis, plot->yAxis);
    QTimer *timer = new QTimer(this);

    QSharedPointer<QCPGraphDataContainer> plotData =
        QSharedPointer<QCPGraphDataContainer>::create();

    qreal currentPlotDataCount = 0;

    int updateInterval = DefaultInterval;
    int bufferSize = DefaultBufferSize;
    bool needsUpdate = false;

    /// update the data on the chart, rescale axes if needed, update the selection and replot
    void updateChart();

public slots:
    void addData(qreal value);
};

#endif
