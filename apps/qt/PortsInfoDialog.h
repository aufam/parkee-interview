#ifndef PORTSINFODIALOG_H
#define PORTSINFODIALOG_H

#include <QDialog>

/// show information about all available serial ports
class PortsInfoDialog : public QDialog {
    Q_OBJECT

public:
    explicit PortsInfoDialog(QWidget *parent = nullptr);
    virtual ~PortsInfoDialog() override;
};

#endif
