import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    id: root
    visible: true
    width: 2560
    height: 1440
    color: "transparent" // 让底层视频透出来

    // 1. 全局样式定义
    property color themeColor: backend.dist < 100 ? "#ff3333" : "#00f0ff"
    property int sensorDist: backend.dist

    // 2. 左下角遮罩（可选）：给摄像头画一个科技感边框
    Rectangle {
        x: 35; y: 675
        width: 1290; height: 730
        color: "transparent"
        border.color: root.themeColor
        border.width: 2
        opacity: 0.5
        
        Text {
            text: "REAR VIEW CAMERA"
            color: root.themeColor
            font.pixelSize: 20
            anchors.bottom: parent.top
            anchors.bottomMargin: 5
        }
    }

    // 3. 右侧数据面板
    Item {
        id: dataPanel
        width: 800
        height: 1000
        anchors.right: parent.right
        anchors.rightMargin: 100
        anchors.verticalCenter: parent.verticalCenter

        // 距离主数值
        Text {
            id: distText
            text: root.sensorDist
            font.pixelSize: 300
            font.bold: true
            color: root.themeColor
            anchors.centerIn: parent
        }

        Text {
            text: "cm"
            font.pixelSize: 80
            color: "white"
            anchors.left: distText.right
            anchors.bottom: distText.bottom
            anchors.bottomMargin: 50
        }

        // 装饰性扫描线
        Rectangle {
            width: 600; height: 4
            color: root.themeColor
            anchors.top: distText.bottom
            anchors.horizontalCenter: distText.horizontalCenter
            
            SequentialAnimation on opacity {
                loops: Animation.Infinite
                NumberAnimation { from: 1.0; to: 0.2; duration: 1000 }
                NumberAnimation { from: 0.2; to: 1.0; duration: 1000 }
            }
        }
    }

    // 4. 顶部状态栏
    Row {
        anchors.top: parent.top
        anchors.topMargin: 50
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 100

        Text {
            text: "SYSTEM READY"
            color: "#00ff41"
            font.pixelSize: 40
        }

        Text {
            text: "TEMP: " + backend.temp + "°C"
            color: "white"
            font.pixelSize: 40
        }

        Text {
            text: Qt.formatDateTime(new Date(), "hh:mm:ss")
            color: "white"
            font.pixelSize: 40
        }
    }
}