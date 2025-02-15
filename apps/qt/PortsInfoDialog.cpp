#include "PortsInfoDialog.h"

#include <QBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QSerialPortInfo>

using namespace Qt::Literals::StringLiterals;

PortsInfoDialog::PortsInfoDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(u"Ports info"_s);

    auto scroll = new QScrollArea(this);
    auto widget = new QWidget(this);
    scroll->setWidget(widget);
    scroll->setWidgetResizable(true);

    auto layout = new QVBoxLayout(widget);
    auto portsInfo = QSerialPortInfo::availablePorts();

    for (const auto &info : portsInfo) if (!info.portName().startsWith(u"ttyS"_s)) {
        const QString infoStr =
            u"Port: "_s + info.portName() + u"\n"_s 
          + u"Location: "_s + info.systemLocation()
          + u"\n"_s + u"Manufacturer: "_s + info.manufacturer() + u"\n"_s 
          + u"Serial number: "_s + info.serialNumber() + u"\n"_s 
          + u"Vendor identifier: "_s + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : u""_s)
          + u"\n"_s + u"Product identifier: "_s + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : u""_s)
          + u"\n"_s
        ;
        
        auto label = new QLabel(infoStr, this);
        layout->addWidget(label);
    }

    auto dialogLayout = new QVBoxLayout(this);
    dialogLayout->addWidget(scroll);
}

PortsInfoDialog::~PortsInfoDialog() {}