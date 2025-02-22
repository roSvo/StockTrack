//INCLUDE QT
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QVector>
#include <QTimer>

//INCLUDE STANDARD

//INCLUDE EXTERNAL
#include <header/stockprotocol.h>

//INLCUDE PROEJCT
#include "header/requesthandler.h"
#include "header/stockcollection.h"

int main(int argc, char *argv[])
{
    //Instead of using QGuiApplication we have to quse QApplication in order to use GuiCharts
    QApplication stockTrackApplication(argc, argv);
    //Load the QML file
    QQmlApplicationEngine engine;

    //Initialize application components
    RequestHandler requestHandler;
    StockCollection stockCollection;

    QObject::connect(&requestHandler, &RequestHandler::stockAdded,
                     &stockCollection, &StockCollection::addStock);

    if(requestHandler.Initialize() == false)
    {
        //Some error handling at some point.
        return 0;
    }

    qmlRegisterType<RequestHandler>("StockTrackClient", 1, 0, "RequestHandler");
    qmlRegisterType<StockCollection>("StockTrackClient", 1, 0, "StockCollection");

    engine.rootContext()->setContextProperty("requestHandler", &requestHandler);
    engine.rootContext()->setContextProperty("stockCollection", &stockCollection);

    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed,
        &stockTrackApplication, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("StockTrackClient", "Main");

    return stockTrackApplication.exec();
}

