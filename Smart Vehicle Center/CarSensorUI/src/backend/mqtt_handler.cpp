#include "mqtt_handler.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

MqttHandler::MqttHandler(QObject *parent) : QObject(parent) {
    m_client = new QMqttClient(this);
    
    // 匹配 MQTTX 截图配置
    m_client->setHostname("broker.emqx.io");
    m_client->setPort(1883);
    m_client->setClientId("mqttx_rk3568_panel"); // 建议加个后缀区分

    connect(m_client, &QMqttClient::stateChanged, this, &MqttHandler::onStateChanged);
    connect(m_client, &QMqttClient::messageReceived, this, &MqttHandler::onMessageReceived);
}

void MqttHandler::connectToBroker() {
    qDebug() << "Connecting to MQTT Broker: broker.emqx.io...";
    m_client->connectToHost();
}

void MqttHandler::onStateChanged() {
    bool isConnected = (m_client->state() == QMqttClient::Connected);
    emit connectionStatusChanged(isConnected);

    if (isConnected) {
        qDebug() << "MQTT Status: Connected!";
        // 订阅一个控制主题，比如控制车载空调或灯光
        m_client->subscribe(QString("rk3568/car/control"));
    } else if (m_client->state() == QMqttClient::Disconnected) {
        qDebug() << "MQTT Status: Disconnected. Reconnecting in 5s...";
    }
}

void MqttHandler::onMessageReceived(const QByteArray &message, const QMqttTopicName &topic) {
    QString msgStr = QString::fromUtf8(message);
    qDebug() << "MQTT Rx [" << topic.name() << "]: " << msgStr;
    emit messageReceived(topic.name(), msgStr);
}

// 核心功能：将 Backend 的传感器数据打包成 JSON 并发送
void MqttHandler::syncTelemetry(Backend *backend) {
    if (!backend) return;

    // 确保只绑定一次
    connect(backend, &Backend::telemetryUpdated, this, [this, backend]() {
        if (m_client->state() != QMqttClient::Connected) {
            // qDebug() << "MQTT Not connected, skipping publish...";
            return;
        }

        QJsonObject json;
        json["speed"] = backend->speed();
        json["distance"] = backend->dist();
        json["temp"] = backend->temp();
        json["status"] = "online";

        QJsonDocument doc(json);
        // 关键：确保 Topic 与 MQTTX 订阅的一致
        QString targetTopic = "rk3568/car/telemetry";
        m_client->publish(targetTopic, doc.toJson(QJsonDocument::Compact));
        
        // 全栈调试打印，确认代码执行到了这里
        qDebug() << ">> [MQTT TX] Topic:" << targetTopic << "Data:" << doc.toJson();
    });
}

void MqttHandler::publishData(const QString &topic, const QByteArray &data) {
    if (m_client->state() == QMqttClient::Connected) {
        m_client->publish(topic, data);
    }
}