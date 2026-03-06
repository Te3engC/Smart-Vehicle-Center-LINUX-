#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include "backend/backend.h"
#include "backend/vedio_provider.h"
#include "backend/mqtt_handler.h" // 确保包含头文件

int main(int argc, char *argv[])
{
    // 强制 GPU 硬件加速平台，不设任何格式限制，让驱动自适应
    qputenv("QT_QPA_PLATFORM", "eglfs");

    QGuiApplication app(argc, argv);

    // 1. 实例化原有功能模块
    Backend backend; 
    VedioProvider videoProvider;

    // 2. 实例化 MQTT 处理器
    MqttHandler mqttHandler;

    // 3. 核心修复：建立 Backend 与 MQTT 的数据同步链路
    // 当 Backend 产生传感器更新信号时，MqttHandler 会自动抓取并发布 JSON
    mqttHandler.syncTelemetry(&backend);

    // 4. 延迟启动 MQTT 连接，确保网络协议栈在 eglfs 加载后完全就绪
    QTimer::singleShot(1500, &mqttHandler, &MqttHandler::connectToBroker);

    // 5. 注册到 QML 上下文
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("backend", &backend);
    engine.rootContext()->setContextProperty("videoProvider", &videoProvider);
    engine.rootContext()->setContextProperty("mqttHandler", &mqttHandler); // 注册后 QML 也能调用

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}