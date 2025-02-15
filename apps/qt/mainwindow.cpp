#include "mainwindow.h"

using namespace Qt::Literals::StringLiterals;

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
    connect(actionSavePlot,     &QAction::triggered, this,               &MainWindow::savePlotData);
    connect(actionSaveImage,    &QAction::triggered, this,               &MainWindow::saveImage);
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

    auto toolBar = new QToolBar(this);
    toolBar->addAction(actionConnect);
    toolBar->addAction(actionDisconnect);
    toolBar->addAction(actionClear);

    addToolBar(Qt::TopToolBarArea, toolBar);
    connect(actionConnect,    &QAction::triggered, this,  &MainWindow::serialConnect);
    connect(actionDisconnect, &QAction::triggered, this,  &MainWindow::serialDisconnect);
    connect(actionClear,      &QAction::triggered, chart, &Chart::clear);

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
        bool ok; auto n = bufferSizeLineEdit->text().toInt(&ok);
        if (ok) chart->setBufferSize(n);
    });

    // side panel: update interval
    auto updateIntervalLabel = new QLabel(u"Update interval (ms):"_s);
    auto updateIntervalLineEdit = new QLineEdit(QString::number(Chart::DefaultInterval));

    sidePanelLayout->addWidget(updateIntervalLabel);
    sidePanelLayout->addWidget(updateIntervalLineEdit);

    connect(updateIntervalLineEdit, &QLineEdit::returnPressed, this, [=, this]() {
        bool ok; auto ms = updateIntervalLineEdit->text().toInt(&ok);
        if (ok) chart->setUpdateInterval(ms);
        else QMessageBox::warning(this, u"Warning"_s,
            u"Cannot set update interval with value: "_s + updateIntervalLineEdit->text()
        );
    });

    // side panel: empty gap
    sidePanelLayout->addStretch();

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
    statusBar->addPermanentWidget(infoWidget);
    infoWidget->setText(u"Info: Disconnected"_s);

    // window
    auto window = new QWidget();
    auto windowLayout = new QHBoxLayout();
    windowLayout->addWidget(sidePanel, 0);
    windowLayout->addWidget(chart, 1);
    window->setLayout(windowLayout);
    setCentralWidget(window);

    // connections
    connect(chart, &Chart::selectedPointChanged, this, [this](const QPointF point) {
        auto msg = u"x: "_s;
        msg += QString::number(point.x());
        msg += u"     y: "_s;
        msg += QString::number(point.y());
        statusBar->showMessage(msg);
    });
    connect(serialTransceiver, &SerialTransceiver::newDataAvailable, chart, &Chart::addData);

    // credits
    connect(actionCredits, &QAction::triggered, this, [this]() {
        auto helpString =
            u"License:\n"_s
            u"qUART is licensed under GPL-3.0\n"_s
            u"Qt 6 is licensed under LGPL-3.0\n"_s
            u"QCustomPlot is licensed under GPL-3.0\n"_s
            u"Material Symbols are licensed under Apache-2.0\n\n"_s
            u"GitHub:\n"_s
            u"https://github.com/ilya-sotnikov/qUART"_s;
        QMessageBox::information(this, u"Credits"_s, helpString);
    });
}

MainWindow::~MainWindow() {}

void MainWindow::serialConnect() {
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
        infoWidget->setText(u"Info: Connected to port "_s + serialSettings.name);
    } else {
        QMessageBox::warning(this, u"Warning"_s, serialTransceiver->errorString());
    }
}

void MainWindow::serialDisconnect() const {
    serialTransceiver->serialClose();
    actionConnect->setEnabled(true);
    actionDisconnect->setEnabled(false);
    actionPortSettings->setEnabled(true);
    actionSavePlot->setEnabled(true);
    actionSaveImage->setEnabled(true);
    dataToSendLineEdit->setEnabled(false);
    infoWidget->setText(u"Info: Disconnected"_s);
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

void MainWindow::saveImage() {
    auto fileName = createFileDialog(QFileDialog::AcceptSave, u"Images (*.png)"_s, u"png"_s);
    if (fileName.isEmpty()) return;

    QPixmap pixMap = chart->grab();
    pixMap.save(fileName, "PNG");
}

void MainWindow::savePlotData() {
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
