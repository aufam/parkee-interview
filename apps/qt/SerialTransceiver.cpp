#include <boost/preprocessor.hpp>
#include <fmt/ranges.h>
#include <fmt/chrono.h>
#include <delameta/debug.h>

#include <parkee-interview/payload.h>
#include <QSharedPointer>
#include "SerialTransceiver.h"

using namespace Qt::Literals::StringLiterals;
using namespace Project;
using parkee::Payload;

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
    auto raw = std::vector<uint8_t>(byteArray.size());
    std::memcpy(raw.data(), byteArray.constData(), byteArray.size());

    auto payload = Payload::deserialize(raw);

    emit emitMessage(QString::fromStdString(
        fmt::format("Raw: {:02X} {}", fmt::join(raw, " "), payload)
    ));

    if (payload.is_ok()) {
        emit emitNewValue(payload.unwrap().value);
        emit emitStats(Payload::get_average(), Payload::get_min(), Payload::get_max());
    }
}

qint64 SerialTransceiver::writeString(const QString &string) {
    return write(string.toUtf8());
}
