#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Chart.h"
#include "SerialTransceiver.h"
#include "PortSettingsDialog.h"
#include "ShowLog.h"
#include "TextWidget.h"
#include "Action.h"

#include <QMainWindow>
#include <QStatusBar>
#include <QFileDialog>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;

private:
    // core
    Chart *chart = new Chart(this);
    SerialTransceiver *serialTransceiver = new SerialTransceiver(this);
    QList<QString> log;

    // menu bar: File
    Action *actionSavePlot = new Action("Save Plot", this);
    Action *actionSaveImage = new Action("Save Image", this);

    // menu bar: Settings
    Action *actionPortSettings = new Action("Port", this);
    PortSettingsDialog *portSettingsDialog = new PortSettingsDialog(this);

    // menu bar: Credits
    Action *actionCredits = new Action("Credits", this);

    // toolbar
    Action *actionConnect = new Action("Connect", this);
    Action *actionDisconnect = new Action("Disconnect", this);
    Action *actionClear = new Action("Clear", this);
    Action *actionRandomize = new Action("Randomize", this);
    Action *actionShowLog = new Action("Show log", this);

    // send data
    QLineEdit *dataToSendLineEdit = new QLineEdit();
    QRadioButton *radioLF = new QRadioButton("LF");
    QRadioButton *radioCR = new QRadioButton("CR");
    QRadioButton *radioCRLF = new QRadioButton("CR+LF");
    QRadioButton *radioNone = new QRadioButton("None");
    QRadioButton *radioHex = new QRadioButton("Hex");
    QString radioText = "CR";

    // stats
    TextWidget *stats = new TextWidget(this);

    // info
    QStatusBar *statusBar = new QStatusBar(this);
    TextWidget *info = new TextWidget(this);

    // helper functions
    void updateStats();

    QString createFileDialog(
        const QFileDialog::AcceptMode acceptMode,
        const QString &nameFilter,
        const QString &defaultSuffix
    );
    std::unique_ptr<QTextStream, std::function<void(QTextStream *)>> openFileStream(
        const QString &fileName,
        QIODevice::OpenMode flags
    );

private slots:
    void onConnect();
    void onDisconnect();
    void onClear();
    void onSaveImage();
    void onSavePlotData();
};

#endif
