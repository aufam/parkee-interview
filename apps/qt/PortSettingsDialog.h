#ifndef PORTSETTINGSDIALOG_H
#define PORTSETTINGSDIALOG_H

#include "PortsInfoDialog.h"

#include <QComboBox>
#include <QDialog>
#include <QSerialPort>

/// settings dialog for serial port
class PortSettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit PortSettingsDialog(QWidget *parent = nullptr);
    virtual ~PortSettingsDialog() override;

    struct Settings {
        QString name;
        QSerialPort::BaudRate baudRate;
        QSerialPort::DataBits dataBits;
        QSerialPort::Parity parity;
        QSerialPort::StopBits stopBits;
        QSerialPort::FlowControl flowControl;
    };

    const Settings &getSettings() const { return settings; };

public slots:
    void showDialog() {
        updateIndexes();
        show();
    }

private:
    PortsInfoDialog *portsInfoDialog = new PortsInfoDialog(this);
    QComboBox *serialPortBox = new QComboBox(this);
    QComboBox *baudRateBox = new QComboBox(this);
    QComboBox *dataBitsBox = new QComboBox(this);
    QComboBox *parityBox = new QComboBox(this);
    QComboBox *stopBitsBox = new QComboBox(this);
    QComboBox *flowControlBox = new QComboBox(this);
    Settings settings;

    // helper functions
    void fillSettings() const;
    void enumeratePorts() const;
    void checkCustomPath(const int index) const;
    void updateSettings();
    void loadSettings();
    void saveSettings() const;
    template<typename T>
    void updateIndex(QComboBox *const comboBox, const T data) const;
    void updateIndexes() const;

private slots:
    void ok();
    void cancel();
};

#endif
