#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QSocketNotifier>
#include <QtSerialPort/QSerialPort>
#include "../common/protocol.h"
class Backend : public QObject {
    Q_OBJECT
    Q_PROPERTY(double speed READ speed NOTIFY telemetryUpdated)
    Q_PROPERTY(int dist READ dist NOTIFY telemetryUpdated)
    Q_PROPERTY(int temp READ temp NOTIFY telemetryUpdated)

public: // <--- 确保这行在这里！！
    explicit Backend(QObject *parent = nullptr);
    ~Backend();

    // 必须是 public，MqttHandler 才能访问
    double speed() const { return m_speed; }
    int dist() const { return m_dist; }
    int temp() const { return m_temp; }

signals:
    void telemetryUpdated();

private slots:
    void handleHardwareRead();
    void handleGpsRead();

private:
    int m_fd;
    QSocketNotifier *m_notifier;
    QSerialPort *m_gpsPort;

    double m_speed = 0;
    int m_dist = 0;
    int m_temp = 0;
};

#endif