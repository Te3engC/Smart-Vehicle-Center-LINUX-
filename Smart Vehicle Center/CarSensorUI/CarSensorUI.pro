# ==============================================================================
# 1. 基础模块配置
# ==============================================================================
# 核心模块：本地编译会自动关联系统路径
QT += core gui qml quick widgets serialport multimedia network

# 针对嵌入式 C++ 标准及大文件支持的优化
CONFIG += c++11
DEFINES += _LARGEFILE_SOURCE _LARGEFILE64_SOURCE _FILE_OFFSET_BITS=64

# ==============================================================================
# 2. 包含路径 (Include Paths)
# ==============================================================================
# 项目源码路径
INCLUDEPATH += $$PWD/src \
               $$PWD/src/backend \
               $$PWD/src/common

# 精准补丁路径：解决板子上找到的 MQTT 和串口位置
# 既然 find 到了 /usr/include/aarch64-linux-gnu/qt5/QtMqtt/QMqttClient
# 我们需要把这一层父目录加入搜索范围
INCLUDEPATH += /usr/include/aarch64-linux-gnu/qt5 \
               /usr/include/aarch64-linux-gnu/qt5/QtMqtt \
               /usr/include/aarch64-linux-gnu/qt5/QtSerialPort

# ==============================================================================
# 3. 库文件链接 (Libraries)
# ==============================================================================
# 1. 链接 MQTT 库：手动编译安装的库通常在 /usr/local/lib 或 /usr/lib/aarch64-linux-gnu
LIBS += -lQt5Mqtt

# 2. 链接 GStreamer (用于 P5V04A/IMX415 视频流) 和原子操作库
LIBS += -latomic \
        -lgstreamer-1.0 -lgstvideo-1.0 -lgstapp-1.0
RESOURCES += qml.qrc
# ==============================================================================
# 4. 源码管理
# ==============================================================================
SOURCES += src/main.cpp \
           src/backend/backend.cpp \
           src/backend/mqtt_handler.cpp \
           src/backend/vedio_provider.cpp

HEADERS += src/backend/backend.h \
           src/backend/mqtt_handler.h \
           src/backend/vedio_provider.h \
           src/common/protocol.h



# ==============================================================================
# 5. 编译输出配置
# ==============================================================================
TARGET = CarSensorUI
TEMPLATE = app