#ifndef STOCKTRACK_REQUESTHANDLER_H
#define STOCKTRACK_REQUESTHANDLER_H

//INCLUDE QT
#include <QObject>
#include <QMap>
#include <QTimer>
#include <QQueue>

//INCLUDE STANDARD LIBRARY

//INCLUDE EXTERNAL
#include <header/stockprotocol.h>

//INCLUDE PROJECT
#include "header/tcpconnect.h"

class RequestHandler : public QObject
{
    Q_OBJECT
public:

    RequestHandler(QObject* parent = nullptr);
    bool Initialize();

//Use SIGNAL and SLOT postfix between C++ communication. Postfixless are saved for QML communication
public slots:
    void addStock(const QString& p_name, const QString& p_symbol, double p_acquisitionPrice);
    void onChartsInitializedSLOT();
    void stockNamesResponseSLOT(QStringList p_stockNames);

signals:
    //Inform stock collection that server has sent stock which should be added to stock collection.
    //This is intended for client to sync it's state with servr
    void stockAddedSIGNAL(const QString& p_name, double p_acquisitionPrice);
    //Update HISOTYR request prices, a situation where server send all daily data of single stock to client as single messge
    void updateMultiplePricesSIGNAL(const QString& p_name, std::vector<std::pair<int, double>> p_prices);
    //Update CURREN_PRICE where only one stock price is sent to client to be updated.
    void updateSinglePriceSIGNAL(const QString& p_name, std::pair<int, double> price);
    //Client is up and running, time to create QML chrats with stock names.
    void initializeChartSIGNAL(QStringList p_stockNames);
    //Request names from StockCollection (internal call)
    void requestStockNamesSIGNAL();
    //Send delete request to server
    void stockDeleteSIGNAL(const QString& p_name);


private slots:

    //Received TCP communication messages are hanlded here.
    void onResponseReceived(const QString& p_response);
    void deleteStock(const QString& p_name);

private:

    //Process history queue one by one.
    void processNextRequest(MessageType p_requestType, std::string p_stockName);
    void requestCurrentPrice();

    //TCP/IP Connection socket
    TCPConnect m_tcpConnect;

    //Stock list responses with all available stocks in database, these should be handled one by one,
    //since each of them requires stock history (stock prices between start and start of the day)
    QQueue<std::string> m_historyRequestQueue;
    //Updating current names on the list requires polling each name at at time.
    QQueue<std::string> m_currentRequestQueue;

    //Timer to hanlder history requests and other stock by stock calls
    QTimer* m_updateTimer;
    //Timer reserved for hourly update calls.
    QTimer* m_requestTimer;

};

#endif // REQUESTHANDLER_H
