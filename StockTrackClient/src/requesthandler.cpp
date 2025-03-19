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

//    connect(m_updateTimer, &QTimer::timeout,
//            this, &RequestHandler::processNextRequest);

    connect(m_requestTimer, &QTimer::timeout,
            this, &RequestHandler::requestCurrentPrice);

    m_requestTimer->start(/*3'600'000*/20'000);
}

bool RequestHandler::Initialize()
{
    std::string stockRequest = StockTrack::Protocol::FormatClientRequest(MessageType::STOCK_LIST);
    m_tcpConnect.sendRequest(QString::fromStdString(stockRequest));
    return true;
}

//SENT REQUESTS
void RequestHandler::addStock(const QString& p_name, const QString& p_symbol, double p_acquisitionPrice)
{
    //Format request with protocol library
    std::string addStockRequest = StockTrack::Protocol::FormatClientRequest(
        MessageType::ADD_STOCK,
        p_name.toStdString(),
        p_symbol.toStdString(),
        p_acquisitionPrice
        );

    //Send to server
    m_tcpConnect.sendRequest(QString::fromStdString(addStockRequest));
}

void RequestHandler::onChartsInitializedSLOT()
{
    if(m_historyRequestQueue.empty() == false)
    {
        processNextRequest(MessageType::HISTORY, m_historyRequestQueue.head());
    }
}

void RequestHandler::stockNamesResponseSLOT(QStringList p_names)
{
    //Here we receive names which stock colleciton has stored to itself.
    // So we should store these into some sort of queue from which we start to poll
    if(p_names.empty() != true)
    {
        for(auto& itr : p_names)
        {
            m_currentRequestQueue.append(itr.toStdString());
        }
    }
    processNextRequest(MessageType::CURRENT_PRICE, m_currentRequestQueue.head());
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
                QStringList stockNames;
                //Iterate through names and store them into a container
                for(auto& itr : message.m_stockNames)
                {
                    //All of these stocks will require "history" check, history request will fill the
                    //prices between start time and date start (from 00.00 to when ever client started)
                    qDebug() << "Adding " << itr << " to history queue";
                    m_historyRequestQueue.append(itr);
                    //For QML, it works best with Qt provided objects.
                    stockNames.append(QString::fromStdString(itr));
                }

                //Send message to QML, this will initialize stock names count amount of empty charts
                emit initializeChartSIGNAL(stockNames);
                break;
            }
            case (MessageType::CURRENT_PRICE):
            {
                if(message.m_stockNames.empty() == false)
                {
                    //Ensure we really have this name in current request queue
                    auto itr = std::find(m_currentRequestQueue.begin(), m_currentRequestQueue.end(),
                                         message.m_stockNames[0]);
                    //Ensure that we have a valid value
                    if(itr == m_currentRequestQueue.end())
                    {
                        //If not, we have not received correct data, wait for another response from server
                        break;
                    }
                    //Erase this from request queue
                    m_currentRequestQueue.erase(itr);

                    //Get the first name from the stock name list. (usually only one)
                    const std::string& stockName = message.m_stockNames[0];

                    //Prices should have only one value, so we can pass what ever is at the back.
                    emit updateSinglePriceSIGNAL(QString::fromStdString(stockName), message.m_prices.back());

                    //Check if we still have more current price requests in queue.
                    if(m_currentRequestQueue.isEmpty() == false)
                    {
                        //If so, process next request, with new values.)
                        processNextRequest(MessageType::CURRENT_PRICE, m_currentRequestQueue.head());
                    }
                }
                break;
            }
            case (MessageType::HISTORY):
            {

                if(message.m_stockNames.empty() == false)
                {
                    //Ensure we really have this name in history queue
                    auto itr = std::find(m_historyRequestQueue.begin(), m_historyRequestQueue.end(),
                                         message.m_stockNames[0]);
                    //Ensure that we have a valid value.
                    if(itr == m_historyRequestQueue.end())
                    {
                        //If not, we have not received correct data, wait for another response froms server.
                        break;
                    }
                    //Erase this from request queue.
                    m_historyRequestQueue.erase(itr);

                    //Get the first name from the stock name list. (usually the only one)
                    const std::string& stockName = message.m_stockNames[0];

                    //Add stock for Client.
                    emit stockAddedSIGNAL(QString::fromStdString(stockName), message.m_acquisitionPrice);

                    //Iterate over all prices.
                    emit updateMultiplePricesSIGNAL(QString::fromStdString(stockName), message.m_prices);

                    //Check if we still have more history prices in queue.
                    if(m_historyRequestQueue.isEmpty() == false)
                    {
                        //If so, process next rqeuest with new value
                        processNextRequest(MessageType::HISTORY, m_historyRequestQueue.head());
                    }
                }
                break;
            }
            case (MessageType::REMOVE_STOCK):
            {
                if(message.m_stockNames.empty() == false)
                {
                    emit stockDeleteSIGNAL(QString::fromStdString(message.m_stockNames[0]));
                }
                break;
            }
            default:
            {
                qDebug("Error received");
                break;
            }
        }
    }
}

void RequestHandler::deleteStock(const QString& p_name)
{
    std::string deleteStockRequest = StockTrack::Protocol::FormatClientRequest(
        MessageType::REMOVE_STOCK,
        p_name.toStdString());

    m_tcpConnect.sendRequest(QString::fromStdString(deleteStockRequest));
}

void RequestHandler::processNextRequest(MessageType p_requestType, std::string p_stockName)
{
    QTimer::singleShot(1000, this, [this, p_requestType, p_stockName]()
    {
        std::string request = StockTrack::Protocol::FormatClientRequest(
            p_requestType,
            p_stockName);

        m_tcpConnect.sendRequest(QString::fromStdString(request));
    });
}

void RequestHandler::requestCurrentPrice()
{
    emit requestStockNamesSIGNAL();
    m_requestTimer->start(3600000);
}



