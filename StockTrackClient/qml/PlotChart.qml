import QtQuick
import QtQuick.Controls
import QtCharts

Item
{
    id: plotChart
    width: 500
    height: 150

    signal deleteStock(string stockName)

    // Pass data to this property from Main.qml
    property string stockName: ""
    property double acquisitionPrice: 0.0
    property bool isInitialized: false
    property var chartData: []

    Connections
    {
        target: stockCollection
        function onStockInitialized(name: string, acqPrice: double): void
        {
            if(name === stockName && !isInitialized)
            {
                //Initial setup
                yAxis.titleText = stockName
                acquisitionPrice = acqPrice

                let range = stockCollection.getPriceRange(stockName)
                yAxis.min = range.min
                yAxis.max = range.max

                isInitialized = true
            }
        }

        function onMultiplePricesUpdated(name: string, prices: Array): void
        {
            if(name === stockName && isInitialized)
            {
                series.clear()
                for(let i = 0; i < prices.length; i++)
                {
                    let point = prices[i]
                    series.append(point.hour, point.price)
                }

                let range = stockCollection.getPriceRange(stockName)
                yAxis.min = range.min ?? 0
                yAxis.max = range.max ?? 10
            }
        }

        function onSinglePriceUpdated(name: string, hour: number, price: number): void
        {

        }
    }

    ChartView
    {
        id: lineChart
        anchors.fill: parent
        antialiasing: true

        //Padding around the chart area
        margins.left: 10
        margins.right: 10
        margins.top: 1
        margins.bottom: 1
        backgroundColor: "transparent"
        opacity: 0.5

        //Remove title to save space
        title: ""

        ValuesAxis
        {
            id: xAxis
            min: 0
            max: 23
            tickCount: 12
            titleText: ""
            labelFormat: "%d"
            gridVisible: true
        }

        ValuesAxis
        {
            id: yAxis
            min: 0
            max: 10



            titleText: stockName
            labelFormat: "%.1f"
        }

        LineSeries
        {
            id: series
            axisX: xAxis
            axisY: yAxis
        }

        //Disalbe the legend (hides the "line series" -title
        legend.visible: false
    }

    Rectangle
    {
        id: closeButton
        width: 15
        height: 15
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 5
        color: closeButtonMouseArea.containsMouse ? "#ff4444" : "#ff6666"

        Text
        {
            anchors.centerIn: parent
            text: "X"
            color: "white"
            font.pixelSize: 16
            font.bold: true
        }

        MouseArea
        {
            id: closeButtonMouseArea
            anchors.fill: parent
            hoverEnabled: true
            onClicked:
            {
                deleteStock(stockName)
            }
        }

    }
}
