#include "backend.h"
#include <fcntl.h>
#include <unistd.h>
#include <QDebug>
#include <QStringList>

Backend::Backend(QObject *parent) : QObject(parent), m_fd(-1) {
    // 1. 环境准备
    system("sudo chmod 666 /dev/car_sensor");
    system("sudo chmod 666 /dev/ttyS3");

    // 2. 初始化传感器驱动 (Character Device)
    m_fd = open("/dev/car_sensor", O_RDONLY | O_NONBLOCK);
    if (m_fd < 0) {
        qCritical() << "Error: Cannot open /dev/car_sensor";
    } else {
        m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
        connect(m_notifier, &QSocketNotifier::activated, this, &Backend::handleHardwareRead);
        qDebug() << "Success: Connected to Kernel Driver.";
    }

    // 3. 初始化 GPS 串口 (UART3)
    m_gpsPort = new QSerialPort(this);
    m_gpsPort->setPortName("/dev/ttyS3");
    m_gpsPort->setBaudRate(QSerialPort::Baud9600); 
    m_gpsPort->setDataBits(QSerialPort::Data8);
    m_gpsPort->setParity(QSerialPort::NoParity);
    m_gpsPort->setStopBits(QSerialPort::OneStop);

    if (m_gpsPort->open(QIODevice::ReadOnly)) {
        connect(m_gpsPort, &QSerialPort::readyRead, this, &Backend::handleGpsRead);
        qDebug() << "Success: GPS Serial Port Opened.";
    } else {
        qCritical() << "Error: Failed to open GPS Port:" << m_gpsPort->errorString();
    }
}

// 处理超声波与温湿度
void Backend::handleHardwareRead() {
    CarSensorPacket packet;
    ssize_t bytes = read(m_fd, &packet, sizeof(CarSensorPacket));
    
    if (bytes == sizeof(CarSensorPacket)) {
        // 更新属性
        m_dist = packet.distance;
        m_temp = packet.temperature;
        
        emit telemetryUpdated(); // 触发 QML 刷新
        qDebug() << "Sensor Update -> Dist:" << m_dist << "cm, Temp:" << m_temp << "C";
    }
}

// 处理 GPS 原始 NMEA 数据
void Backend::handleGpsRead() {
    while (m_gpsPort->canReadLine()) {
        QByteArray data = m_gpsPort->readLine().trimmed();
        QString line = QString::fromLocal8Bit(data);

        // 解析 $GPVTG 语句获取速度 (km/h)
        if (line.startsWith("$GPVTG")) {
            QStringList parts = line.split(",");
            if (parts.size() >= 8) {
                double speedVal = parts.at(7).toDouble();
                if (speedVal >= 0) {
                    m_speed = speedVal;
                    emit telemetryUpdated();
                    qDebug() << "GPS Speed ->" << m_speed << "km/h";
                }
            }
        }
        
        // 解析 $GPRMC 获取定位状态 (可选)
        if (line.startsWith("$GPRMC")) {
            QStringList parts = line.split(",");
            if (parts.size() >= 3 && parts.at(2) == "V") {
                // V 代表无效信号（通常在室内搜不到星时出现）
                // qDebug() << "GPS Warning: Signal Invalid (In Door?)";
            }
        }
    }
}

Backend::~Backend() {
    if (m_fd >= 0) close(m_fd);
    if (m_gpsPort && m_gpsPort->isOpen()) m_gpsPort->close();
}