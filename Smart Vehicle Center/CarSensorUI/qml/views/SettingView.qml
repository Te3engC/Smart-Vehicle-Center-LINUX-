import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: settingView

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.7
    }

    Column {
        anchors.centerIn: parent
        spacing: 30

        Label {
            text: "智能车机系统设置"
            color: "white"; font.pixelSize: 40; font.bold: true
        }

        // MQTT 连接控制
        Button {
            text: "连接远程监控中心"
            width: 300; height: 60
            onClicked: mqttHandler.connectToBroker("192.168.1.100", 1883)
        }

        // 视频质量调节
        Row {
            spacing: 20
            Text { text: "摄像头流质量:"; color: "white"; font.pixelSize: 24 }
            ComboBox {
                model: ["720P (流畅)", "1080P (高清)", "4K (硬解)"]
                currentIndex: 0
            }
        }

        Text {
            text: "固件版本: V1.0.2 (RK3568-ARM64)"
            color: "gray"; font.pixelSize: 18
        }
    }
}
