import QtQuick
import QtQuick.Controls
import QtCharts

Item
{
    id: plotChart
    width: 500
    height: 150

    // Pass data to this property from Main.qml
    property string stockName: ""
    property var chartData: []

    onChartDataChanged:
    {
        series.clear()

        if(chartData && chartData.length > 0)
        {
            let range = stockCollection.getPriceRange(stockName);
            console.log("Range for ", stockName, " - min:", range.min, " max: ", range.max)

            yAxis.min = range.min;
            yAxis.max = range.max;

            //Plot non zero values
            for(let hour = 0; hour < chartData.length; hour++)
            {
                if(chartData[hour] > 0)
                {
                    series.append(hour, chartData[hour])
                }
            }
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

        //Remove title to save space
        title: ""

        ValuesAxis
        {
            id: xAxis
            min: 0
            max: 23
            titleText: ""
            labelFormat: "%d"
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
}
