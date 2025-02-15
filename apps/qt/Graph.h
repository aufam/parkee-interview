#ifndef CUSTOMGRAPH_H
#define CUSTOMGRAPH_H

#include "qcustomplot.h"

/// graph implementation from QCPGraph
class CustomGraph : public QCPGraph {
public:
    CustomGraph(QCPAxis *keyAxis, QCPAxis *valueAxis);
    virtual ~CustomGraph() override;

private:
    using QCPGraph::QCPGraph;
};

#endif
