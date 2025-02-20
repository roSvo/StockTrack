import QtQuick
import QtQuick.Controls
import QtCharts
import StockTrackClient 1.0

ApplicationWindow
{
    id: stockTrackWindow
    width: 500
    height: 700
    visible: true
    title: "Stock Track Client"

    property int currentPage: 0
    property int chartsPerPage: 4

    Item
    {
        anchors.fill: parent

        //Chart area
        Column
        {
            id: plotArea
            width: parent.width
            height: parent.height
            spacing: 1

            Repeater
            {
                model: stockCollection.stockNames.slice(currentPage * chartsPerPage, (currentPage + 1) * chartsPerPage);                //model: stockData.stockNames.slice(currentPage * chartsPerPage, (currentPage + 1) * chartsPerPage)

                delegate: PlotChart
                {
                    width: parent.width
                    stockName: modelData
                    chartData: stockCollection.getPrices[modelData] || []

                    Component.onCompleted:
                    {
                        console.log("Stock name : ", modelData)
                    }
                }
            }
        }

        // Navigation Buttons
        Row
        {
            id: navButtons
            spacing: 5
            height: 50
            anchors {
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
            }
            anchors.bottomMargin: 10

            Button
            {
                text: "<<"
                onClicked: currentPage = Math.max(currentPage - 1, 0)
            }
            Button
            {
                text: "Add Stock"
                onClicked: addStockDialog.open()
            }

            Button
            {
                text: "Fetch List"
                onClicked:
                {
                    console.log("Button clicked")
                }
            }
            Button
            {
                text: ">>"
                onClicked: currentPage = Math.min(currentPage + 1, Math.ceil(plotData.length / chartsPerPage) - 1)
            }
        }

        //New dialog for adding stocks.
        Dialog
        {
            id: addStockDialog
            title: "Add New Stock"
            standardButtons: Dialog.Ok | Dialog.Cancel
            modal: true

            Column
            {
                spacing: 10
                TextField
                {
                    id: nameField
                    placeholderText: "Stock Name"
                }
                TextField
                {
                    id: symbolField
                    placeholderText: "SYMBOL"
                }
                TextField
                {
                    id: acquisitionField
                    placeholderText: "Acquisation Price"
                    validator: DoubleValidator{
                        decimals: 2
                        notation: DoubleValidator.StandardNotation
                        bottom: 0.00
                        top: 999999.99
                    }
                    //Replace comma with period on input:
                    onTextChanged: {
                        text = text.replace(",", ".")
                    }
                }
            }

            onAccepted:
            {
                requestHandler.addStock(
                            nameField.text,
                            symbolField.text,
                            Number(acquisitionField.text))

                nameField.clear()
                symbolField.clear()
                acquisitionField.clear()
            }
            onRejected:
            {
                nameField.clear()
                symbolField.clear()
                acquisitionField.clear()
            }
        }
    }

    Component.onCompleted:
    {
        // Position window on the second monitor
        var screens = Qt.application.screens;

        if (screens.length > 1)
        {
            stockTrackWindow.x = screens[0].width;
            stockTrackWindow.y = screens[1].virtualY;
        }
        else
        {
            stockTrackWindow.x = 0;
            stockTrackWindow.y = 0;
        }

        console.log(("Request stock list FROM QML."))
        //TCPConnect.requestStockList()

    }
}
