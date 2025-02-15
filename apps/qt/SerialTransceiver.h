#ifndef SERIALTRANSCEIVER_H
#define SERIALTRANSCEIVER_H

#include <QSerialPort>

// serial port implementation from QSerialPort
class SerialTransceiver : public QSerialPort {
    Q_OBJECT

public:
    explicit SerialTransceiver(QObject *parent = nullptr);
    virtual ~SerialTransceiver() override;

    bool serialOpen();
    void serialClose();

    qint64 writeString(const QString &string);

private:
    void deserializeByteArray(QByteArray &byteArray, QList<qreal> &dataList);

private slots:
    void receiveData();

signals:
    void newDataAvailable(const QSharedPointer<const QList<qreal>> dataList);
};

#endif
