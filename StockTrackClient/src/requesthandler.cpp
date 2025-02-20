//INCLUDE QT
#include <QDebug>
#include <QDateTime>

//INCLUDE STANDARD LIBRARY

//INCLUDE EXTERNAL

//INCLUDE PROJECT
#include "header/requesthandler.h"

RequestHandler::RequestHandler(QObject* parent)
    : QObject(parent)
    , m_tcpConnect(this)
{
    m_updateTimer = new QTimer(this);
    m_requestTimer = new QTimer(this);

    connect(&m_tcpConnect, &TCPConnect::dataReceived,
            this, &RequestHandler::onResponseReceived);

}

TCPConnect* RequestHandler::getTCPConnect()
{
    return &m_tcpConnect;
}


//SENT REQUESTS
void RequestHandler::addStock(const QString& name, const QString& symbol, double acquisitionPrice)
{
    //Format request using protocol
    qDebug() << "Name : " << name;

    std::string addStockRequest = StockTrack::Protocol::FormatClientRequest(
        MessageType::ADD_STOCK,
        name.toStdString(),
        symbol.toStdString(),
        acquisitionPrice
        );

    qDebug() << "Protocol string : " << addStockRequest;

    //Send to server
    m_tcpConnect.sendRequest(QString::fromStdString(addStockRequest));
}


//RECEIVED REQUESTS
void RequestHandler::onResponseReceived(const QString& p_response)
{

    //Unify to be std string.
    std::string stringResponse = p_response.toStdString();
    StockTrack::Message message = StockTrack::Protocol::ParseMessage(stringResponse);

    if(message.m_success)
    {
        switch(message.m_messageType)
        {
            case (MessageType::ADD_STOCK) :
            {
                break;
            }
            case (MessageType::STOCK_LIST):
            {

                break;
            }
            case (MessageType::CURRENT_PRICE):
            {

                break;
            }
            case (MessageType::HSOTRY):
            {

                break;
            }
            default:
            {
                break;
            }
        }


    }

}



