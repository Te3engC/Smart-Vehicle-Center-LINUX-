import QtQuick 2.12

Canvas {
    id: gaugeCanvas
    width: 400
    height: 400

    // 定义对外接口：接收来自 backend 的数值
    property real value: 0
    property string unit: "KM/H"
    property color themeColor: value > 100 ? "#ff4444" : "#00f0ff"

    // 当数值改变时，自动触发重绘
    onValueChanged: requestPaint()

    onPaint: {
        var ctx = getContext("2d");
        var centerX = width / 2;
        var centerY = height / 2;
        var radius = Math.min(width, height) / 2 - 20;

        ctx.reset();

        // 1. 绘制背景底环
        ctx.beginPath();
        ctx.strokeStyle = "#222222";
        ctx.lineWidth = 15;
        ctx.arc(centerX, centerY, radius, 0.8 * Math.PI, 2.2 * Math.PI);
        ctx.stroke();

        // 2. 绘制动态数值环
        // 将数值映射到角度 (假设最大值 220)
        var progress = Math.min(value / 220, 1.0);
        var endAngle = 0.8 * Math.PI + (progress * 1.4 * Math.PI);

        ctx.beginPath();
        ctx.strokeStyle = themeColor;
        ctx.lineWidth = 15;
        ctx.lineCap = "round";
        ctx.arc(centerX, centerY, radius, 0.8 * Math.PI, endAngle);
        ctx.stroke();

        // 3. 绘制中间文字
        ctx.fillStyle = "white";
        ctx.font = "bold 60px Arial";
        ctx.textAlign = "center";
        ctx.fillText(Math.floor(value), centerX, centerY + 10);

        ctx.font = "20px Arial";
        ctx.fillStyle = "#aaaaaa";
        ctx.fillText(unit, centerX, centerY + 50);
    }
}
