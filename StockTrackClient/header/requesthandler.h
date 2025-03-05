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
#include "header/stockcollection.h"

class RequestHandler : public QObject
{
    Q_OBJECT
public:

    RequestHandler(QObject* parent = nullptr);
    bool Initialize();

    TCPConnect* getTCPConnect();
    StockCollection* getStockData();

public slots:
    void addStock(const QString& p_name, const QString& p_symbol, double p_acquisitionPrice);

    void onChartsInitializedSLOT();

signals:
    void stockAddedSIGNAL(const QString& p_name, double p_acquisitionPrice);
    void updateMultiplePricesSIGNAL(const QString& p_name, std::vector<std::pair<int, double>> p_prices);
    void updateSinglePriceSIGNAL(const QString& p_name, int p_hour, double p_price);
    void initializeChartSIGNAL(QStringList p_stockNames);

private slots:

    void onResponseReceived(const QString& p_response);

private:

    //Process history queue one by one.
    void processNextHistoryRequest();

    //TCP/IP Connection socket
    TCPConnect m_tcpConnect;

    //Stock list responses with all available stocks in database, these should be handled one by one,
    //since each of them requires stock history (stock prices between start and start of the day)
    QQueue<std::string> m_historyRequestQueue;

    //Timer to hanlder history requests and other stock by stock calls
    QTimer* m_updateTimer;
    //Timer reserved for hourly update calls.
    QTimer* m_requestTimer;



};

#endif // REQUESTHANDLER_H
