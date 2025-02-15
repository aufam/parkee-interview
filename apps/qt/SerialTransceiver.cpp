#include <QSharedPointer>
#include "SerialTransceiver.h"

using namespace Qt::Literals::StringLiterals;

SerialTransceiver::SerialTransceiver(QObject *parent) : QSerialPort(parent) {
    connect(this, &QSerialPort::readyRead, this, &SerialTransceiver::receiveData);
}

SerialTransceiver::~SerialTransceiver() {}

bool SerialTransceiver::serialOpen() {
    if (open(QIODevice::ReadWrite)) {
        clear();
    }

    return isOpen();
}

void SerialTransceiver::serialClose() {
    if (isOpen()) close();
}

void SerialTransceiver::deserializeByteArray(QByteArray &byteArray, QList<qreal> &dataList) {
    for (auto byte: byteArray) {
        dataList.append(byte / 128.0);
    }
}

void SerialTransceiver::receiveData() {
    auto byteArray = readAll();
    QList<qreal> dataList;

    deserializeByteArray(byteArray, dataList);

    if (!dataList.isEmpty()) {
        auto dataShared = QSharedPointer<QList<qreal>>::create(dataList);
        emit newDataAvailable(dataShared);
    }
}

qint64 SerialTransceiver::writeString(const QString &string) {
    return write(string.toUtf8());
}
