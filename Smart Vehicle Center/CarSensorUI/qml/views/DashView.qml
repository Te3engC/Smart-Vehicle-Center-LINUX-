import QtQuick 2.12
import "../components" // 自动匹配目录下的组件

Item {
    id: dashView
    anchors.fill: parent

    // 动态仪表组件
    GaugeItem {
        id: speedGauge
        anchors.centerIn: parent
        // 绑定 C++ 后端的 GPS 航速
        value: backend.speed
    }

    // 状态叠加层
    Column {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 40
        spacing: 15

        Text {
            text: "车内温度: " + backend.temp + "°C"
            color: "cyan"; font.pixelSize: 24
        }
        Text {
            text: "避障距离: " + backend.dist + "cm"
            color: backend.dist < 30 ? "red" : "white" // 红色预警逻辑
            font.pixelSize: 24
        }
    }
}
