import QtQuick 2.12
import QtQuick.Controls 2.12

Item {
    id: mapView
    anchors.fill: parent

    // 地图背景占位（未来可接入 Map 模块）
    Rectangle {
        anchors.fill: parent
        color: "#1a1a2e"
        opacity: 0.6
    }

    // GPS 信息叠加层
    Column {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 50
        spacing: 15

        Text {
            text: "卫星状态: " + (backend.speed > 0 ? "已定位" : "搜索中...")
            color: "#00f0ff"; font.pixelSize: 28
        }
        Text {
            text: "当前航速: " + backend.speed.toFixed(1) + " KM/H"
            color: "white"; font.pixelSize: 42
        }
    }

    // 模拟导航罗盘
    Image {
        source: "qrc:/assets/compass.png" // 对应你的 assets 目录
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 50
        width: 150; height: 150
    }
}
