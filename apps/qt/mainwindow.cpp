#include <parkee-interview/payload.h>
#include "mainwindow.h"

using namespace Qt::Literals::StringLiterals;
using namespace Project;
using parkee::Payload;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setMinimumSize(QSize{ 800, 600 });
    setWindowTitle(u""_s PARKEE_PROJECT_NAME u" -- v"_s PARKEE_PROJECT_VERSION);
    setWindowIcon(QIcon{ u":app.png"_s });

    // menu bar
    auto menuBar = new QMenuBar(this);
    auto menuFile = new QMenu(u"File"_s, menuBar);
    auto menuSettings = new QMenu(u"Settings"_s, menuBar);
    auto menuHelp = new QMenu(u"Help"_s, menuBar);

    menuBar->addAction(menuFile->menuAction());
    menuBar->addAction(menuSettings->menuAction());
    menuBar->addAction(menuHelp->menuAction());
    menuFile->addAction(actionSavePlot);
    menuFile->addAction(actionSaveImage);
    menuSettings->addAction(actionPortSettings);
    menuHelp->addAction(actionCredits);

    setMenuBar(menuBar);
    connect(actionSavePlot,     &QAction::triggered, this,               &MainWindow::onSavePlotData);
    connect(actionSaveImage,    &QAction::triggered, this,               &MainWindow::onSaveImage);
    connect(actionPortSettings, &QAction::triggered, portSettingsDialog, &PortSettingsDialog::showDialog);

    // Toolbar
    actionConnect->setEnabled(true);
    actionDisconnect->setEnabled(false);

    actionConnect->setIcon(u":actionConnect.png"_s);
    actionConnect->setShortcut(QKeySequence(u"Alt+C"_s));
    actionDisconnect->setIcon(u":actionDisconnect.png"_s);
    actionDisconnect->setShortcut(QKeySequence(u"Alt+D"_s));
    actionClear->setIcon(u":actionClear.png"_s);
    actionClear->setShortcut(QKeySequence(u"Ctrl+L"_s));
    actionRandomize->setIcon(u":actionAppendToPlot.png"_s);
    actionRandomize->setShortcut(QKeySequence(u"Alt+R"_s));

    auto toolBar = new QToolBar(this);
    toolBar->addAction(actionConnect);
    toolBar->addAction(actionDisconnect);
    toolBar->addAction(actionClear);
    toolBar->addAction(actionRandomize);

    addToolBar(Qt::TopToolBarArea, toolBar);
    connect(actionConnect,    &QAction::triggered, this, &MainWindow::onConnect);
    connect(actionDisconnect, &QAction::triggered, this, &MainWindow::onDisconnect);
    connect(actionClear,      &QAction::triggered, this, &MainWindow::onClear);
    connect(actionRandomize,  &QAction::triggered, this, [this] {
        chart->addData(Payload::Random().value);
        updateStats();
    });

    // side panel
    auto sidePanel = new QGroupBox(); // will be added to window layout
    auto sidePanelLayout = new QVBoxLayout();
    sidePanel->setLayout(sidePanelLayout);

    // side panel: buffer size
    auto bufferSizeLabel = new QLabel(u"Buffer size:"_s);
    auto bufferSizeLineEdit = new QLineEdit(QString::number(Chart::DefaultBufferSize));

    sidePanelLayout->addWidget(bufferSizeLabel);
    sidePanelLayout->addWidget(bufferSizeLineEdit);

    connect(bufferSizeLineEdit, &QLineEdit::returnPressed, this, [=, this]() {
        const auto& text = bufferSizeLineEdit->text();
        bool ok; auto n = text.toInt(&ok);
        if (ok and n > 0) {
            chart->setBufferSize(n);
            info->setText(u"Buffer size is set to "_s + text);
        } else QMessageBox::warning(this, u"Warning"_s,
            u"Cannot set buffer size with value: "_s + text
        );
    });

    // side panel: update interval
    auto updateIntervalLabel = new QLabel(u"Update interval (ms):"_s);
    auto updateIntervalLineEdit = new QLineEdit(QString::number(Chart::DefaultInterval));

    sidePanelLayout->addWidget(updateIntervalLabel);
    sidePanelLayout->addWidget(updateIntervalLineEdit);

    connect(updateIntervalLineEdit, &QLineEdit::returnPressed, this, [=, this]() {
        const auto& text = updateIntervalLineEdit->text();
        bool ok; auto ms = text.toInt(&ok);
        if (ok and ms > 0) {
            chart->setUpdateInterval(ms);
            info->setText(u"Update tnterval is set to "_s + text + u"ms"_s);
        }
        else QMessageBox::warning(this, u"Warning"_s,
            u"Cannot set update interval with value: "_s + text
        );
    });

    // side panel: stats
    sidePanelLayout->addWidget(stats);
    updateStats();
    sidePanelLayout->addStretch(); // empty gap

    // side panel: data to send
    auto dataToSendLabel = new QLabel(u"Send:"_s);
    dataToSendLineEdit->setEnabled(false); // disable at first

    auto radioButtonsLayout = new QVBoxLayout();
    radioButtonsLayout->addWidget(radioLF);
    radioButtonsLayout->addWidget(radioCR);
    radioButtonsLayout->addWidget(radioCRLF);
    radioButtonsLayout->addWidget(radioNone);
    radioButtonsLayout->addWidget(radioHex);

    radioLF->setChecked(true);

    sidePanelLayout->addWidget(dataToSendLabel);
    sidePanelLayout->addWidget(dataToSendLineEdit);
    sidePanelLayout->addLayout(radioButtonsLayout);

    auto *radioGroup = new QButtonGroup(this);
    radioGroup->addButton(radioLF);
    radioGroup->addButton(radioCR);
    radioGroup->addButton(radioCRLF);
    radioGroup->addButton(radioNone);
    radioGroup->addButton(radioHex);

    connect(radioGroup, &QButtonGroup::buttonClicked, this, [this](QAbstractButton *button) {
        radioText = button->text();
    });

    connect(dataToSendLineEdit, &QLineEdit::returnPressed, this, [this]() {
        auto text = dataToSendLineEdit->text();
        if (radioText == "LF") {
            text += '\n';
            serialTransceiver->writeString(text);
        } else if (radioText == "CR") {
            text += '\r';
            serialTransceiver->writeString(text);
        } else if (radioText == "CRLF") {
            text += "\r\n";
            serialTransceiver->writeString(text);
        } else if (radioText == "None") {
            serialTransceiver->writeString(text);
        } else if (radioText == "Hex") {
            // TODO: implement convert to bytes
            QMessageBox::warning(this, u"Warning"_s, "Not implemented yet");
        }
    });

    // status bar
    setStatusBar(statusBar);
    statusBar->addPermanentWidget(info);

    // window
    auto window = new QWidget();
    auto windowLayout = new QHBoxLayout();
    windowLayout->addWidget(sidePanel, 0);
    windowLayout->addWidget(chart, 1);
    window->setLayout(windowLayout);
    setCentralWidget(window);

    // connections
    connect(serialTransceiver, &SerialTransceiver::emitNewValue, chart, &Chart::addData);
    connect(serialTransceiver, &SerialTransceiver::emitMessage, this, [this](QString msg) {
        info->setText(msg);
        updateStats();
    });

    // credits
    connect(actionCredits, &QAction::triggered, this, [this]() {
        auto credits =
            u"Plotting and Data Visualization:\n"_s
            u"https://www.qcustomplot.com/\n\n"_s
            u"Icons:\n"_s
            u"https://github.com/ilya-sotnikov/qUART/tree/main/icons"_s;
        QMessageBox::information(this, u"Credits"_s, credits);
    });
}

MainWindow::~MainWindow() {}

void MainWindow::updateStats() {
    stats->setText(
        u"Average = "_s + QString::number(Payload::get_average(), 'f', 3) + u"\n"_s +
        u"Min = "_s + QString::number(Payload::get_min(), 'f', 3) + u"\n"_s +
        u"Max = "_s + QString::number(Payload::get_max(), 'f', 3) + u"\n"_s
    );
}

void MainWindow::onConnect() {
    const auto &serialSettings = portSettingsDialog->getSettings();

    serialTransceiver->setPortName(serialSettings.name);
    serialTransceiver->setBaudRate(serialSettings.baudRate);
    serialTransceiver->setDataBits(serialSettings.dataBits);
    serialTransceiver->setParity(serialSettings.parity);
    serialTransceiver->setStopBits(serialSettings.stopBits);
    serialTransceiver->setFlowControl(serialSettings.flowControl);

    if (serialTransceiver->serialOpen()) {
        actionConnect->setEnabled(false);
        actionDisconnect->setEnabled(true);
        actionPortSettings->setEnabled(false);
        actionSavePlot->setEnabled(false);
        actionSaveImage->setEnabled(false);
        dataToSendLineEdit->setEnabled(true);
        info->setText(u"Connected to port "_s + serialSettings.name);
    } else {
        QMessageBox::warning(this, u"Warning"_s, serialTransceiver->errorString());
    }
}

void MainWindow::onDisconnect() {
    serialTransceiver->serialClose();
    actionConnect->setEnabled(true);
    actionDisconnect->setEnabled(false);
    actionPortSettings->setEnabled(true);
    actionSavePlot->setEnabled(true);
    actionSaveImage->setEnabled(true);
    dataToSendLineEdit->setEnabled(false);
    info->setText(u"Disconnected"_s);
}

void MainWindow::onClear() {
    chart->clear();
    Payload::reset(); // reset statistic
    info->setText(u"Buffer cleared"_s);
    updateStats();
}

QString MainWindow::createFileDialog(QFileDialog::AcceptMode acceptMode, const QString &nameFilter, const QString &defaultSuffix) {
    auto fileDialog = QFileDialog(this);
    fileDialog.setAcceptMode(acceptMode);
    fileDialog.setNameFilter(nameFilter);
    fileDialog.setDefaultSuffix(defaultSuffix);

    if (!fileDialog.exec() || fileDialog.selectedFiles().isEmpty())
        return u""_s;
    else
        return fileDialog.selectedFiles().at(0);
}

std::unique_ptr<QTextStream, std::function<void(QTextStream *)>> MainWindow::openFileStream(
        const QString &fileName,
        QIODevice::OpenMode fileFlags
) {
    auto file = new QFile(fileName);
    auto fileDeleter = [file](QTextStream *stream) {
        delete stream;
        file->deleteLater();
    };

    if (!file->open(fileFlags)) {
        QMessageBox::warning(this, u"Warning"_s, file->errorString());
        file->deleteLater();
        return { nullptr };
    } else {
        return { new QTextStream(file), fileDeleter };
    }
}

void MainWindow::onSaveImage() {
    auto fileName = createFileDialog(QFileDialog::AcceptSave, u"Images (*.png)"_s, u"png"_s);
    if (fileName.isEmpty()) return;

    QPixmap pixMap = chart->grab();
    pixMap.save(fileName, "PNG");
}

void MainWindow::onSavePlotData() {
    auto fileName = createFileDialog(QFileDialog::AcceptSave, u"Text files (*.txt)"_s, u"txt"_s);
    if (fileName.isEmpty())
        return;

    auto stream = openFileStream(fileName, QIODevice::WriteOnly | QIODevice::Text);
    if (stream.get() == nullptr)
        return;

    const auto& dataList = chart->getPlotData();
    for (const auto &data : *dataList)
        *stream << QString::number(data.mainValue()) << u"\n"_s;
}
