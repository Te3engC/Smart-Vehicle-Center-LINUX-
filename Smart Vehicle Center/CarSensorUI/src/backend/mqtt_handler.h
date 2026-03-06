#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <QObject>
#include <QtMqtt/QMqttClient>
#include <QtMqtt/QMqttTopicName>
#include "backend.h"

class MqttHandler : public QObject {
    Q_OBJECT
public:
    explicit MqttHandler(QObject *parent = nullptr);
    
    // 初始化连接（对应 MQTTX 截图参数）
    void connectToBroker();
    
    // 手动发布数据接口
    void publishData(const QString &topic, const QByteArray &data);
    
    // 自动同步传感器数据到云端
    void syncTelemetry(Backend *backend);

signals:
    void messageReceived(const QString &topic, const QString &message);
    void connectionStatusChanged(bool connected);

private slots:
    void onStateChanged();
    void onMessageReceived(const QByteArray &message, const QMqttTopicName &topic);

private:
    QMqttClient *m_client;
};

#endif