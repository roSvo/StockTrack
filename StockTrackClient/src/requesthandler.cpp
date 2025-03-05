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
        processNextHistoryRequest();
    }
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

                break;
            }
            case (MessageType::HISTORY):
            {

                if(message.m_stockNames.empty() == false)
                {
                    //Ensure we really have this name in history queue
                    auto itr = std::find(m_historyRequestQueue.begin(), m_historyRequestQueue.end(),
                                         message.m_stockNames[0]);
                    //Check that we have a valid value.
                    if(itr == m_historyRequestQueue.end())
                    {
                        //If not, we have not received correct data, wait for another connection attempt.
                        break;
                    }
                    //Erase this from request queue.
                    m_historyRequestQueue.erase(itr);

                    //Get the first name fro mthe stock name list.
                    const std::string& stockName = message.m_stockNames[0];

                    //Add stock for Client.
                    emit stockAddedSIGNAL(QString::fromStdString(stockName), message.m_acquisitionPrice);

                    //Iterate over all prices.
                    emit updateMultiplePricesSIGNAL(QString::fromStdString(stockName), message.m_prices);

                    processNextHistoryRequest();
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

    qDebug() << "Sending history request : " << m_historyRequestQueue;
    if(m_historyRequestQueue.isEmpty() == false)
    {
        QTimer::singleShot(1000, this, [this]()
        {
            std::string stockName = m_historyRequestQueue.head();
            std::string historyRequest = StockTrack::Protocol::FormatClientRequest(
                MessageType::HISTORY,
                stockName);

            m_tcpConnect.sendRequest(QString::fromStdString(historyRequest));
        });
    }
}



