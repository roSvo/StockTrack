#ifndef STOCKTRACK_REQUESTHANDLER_H
#define STOCKTRACK_REQUESTHANDLER_H

//INCLUDE QT
#include <QObject>
#include <QMap>
#include <QTimer>

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

    TCPConnect* getTCPConnect();
    StockCollection* getStockData();

    Q_INVOKABLE void addStock(const QString& name, const QString& symbol, double acquisationPrice);

signals:
    void stockAdded(const QString& name, double acquisitionPrice);
    void priceUpdate(const QString& name, int hour, double price);

private slots:

    void onResponseReceived(const QString& p_response);

private:

    void startHourlyUpdate();

    TCPConnect m_tcpConnect;

    //Timer to hanlder history requests and other stock by stock calls
    QTimer* m_updateTimer;

    //Timer reserved for hourly update calls.
    QTimer* m_requestTimer;

};

#endif // REQUESTHANDLER_H
