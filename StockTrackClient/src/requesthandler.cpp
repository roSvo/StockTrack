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

    connect(m_updateTimer, &QTimer::timeout,
            this, &RequestHandler::processNextHistoryRequest);

}

bool RequestHandler::Initialize()
{
    std::string stockRequest = StockTrack::Protocol::FormatClientRequest(MessageType::STOCK_LIST);
    m_tcpConnect.sendRequest(QString::fromStdString(stockRequest));

    return true;
}

TCPConnect* RequestHandler::getTCPConnect()
{
    return &m_tcpConnect;
}


//SENT REQUESTS
void RequestHandler::addStock(const QString& name, const QString& symbol, double acquisitionPrice)
{
    //Format request with protocol library
    std::string addStockRequest = StockTrack::Protocol::FormatClientRequest(
        MessageType::ADD_STOCK,
        name.toStdString(),
        symbol.toStdString(),
        acquisitionPrice
        );

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
                //Iterate through names and store them into a container
                for(auto& itr : message.m_stockNames)
                {
                    //All of these stocks will require "history" check, history request will fill the
                    //prices between start time and date start (from 00.00 to when ever client started)
                    m_historyRequestQueue.append(itr);
                }
                //In order to keep things going, wait for a bit before filling history.
                if(m_updateTimer->isActive() == false)
                {
                    m_updateTimer->start(1000);
                }

                break;
            }
            case (MessageType::CURRENT_PRICE):
            {

                break;
            }
            case (MessageType::HISTORY):
            {
                if(message.m_stockNames.empty() == false)
                {
                    const std::string& stockName = message.m_stockNames[0];
                    emit stockAddedSIGNAL(QString::fromStdString(stockName), message.m_acquisitionPrice);

                    for(const auto& itr : message.m_prices)
                    {
                        emit updatePriceSIGNAL(QString::fromStdString(stockName), itr.first, itr.second);
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
}

void RequestHandler::processNextHistoryRequest()
{
    qDebug() << "Sending history request.";
    if(m_historyRequestQueue.isEmpty() == false)
    {
        std::string stockName = m_historyRequestQueue.dequeue();
        std::string historyRequest = StockTrack::Protocol::FormatClientRequest(
            MessageType::HISTORY,
            stockName);

        m_tcpConnect.sendRequest(QString::fromStdString(historyRequest));

    }
    else
    {
        m_updateTimer->stop();
    }
}



