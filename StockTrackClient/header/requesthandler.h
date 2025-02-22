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


    Q_INVOKABLE void addStock(const QString& name, const QString& symbol, double acquisationPrice);

signals:
    void stockAdded(const QString& name, double acquisitionPrice);
    void priceUpdate(const QString& name, int hour, double price);

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
