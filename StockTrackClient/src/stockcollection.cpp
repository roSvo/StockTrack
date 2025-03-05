//INCLUDE QT
#include <QDebug>
#include <QVariant>
//INCLUDE STANDARD LIBRARY

//INCLUDE EXTERNAL

//INCLUDE PROJECT
#include "header/stockcollection.h"

StockCollection::StockCollection(QObject* p_parent)
    : QObject(p_parent),
    m_stocks{}
{

}

QStringList StockCollection::getStockNames() const
{
    QStringList names;
    for(auto& itr : m_stocks)
    {
        names.append(itr.m_name);
    }
    return names;
}

QList<double> StockCollection::getPrices(const QString& p_name) const
{
    /*
    QList<double> prices;



    if(stock.m_name == name)
    {
        qDebug() << "Getting prices for" << name << "with" << stock.m_prices.size() << "entries";
        for(unsigned int hour = 0; hour < 24; ++hour)
        {
            double price = stock.m_prices.value(hour, 0.0);
            prices.append(price);
            if(price > 0)
            {
                qDebug() << "Hour:" << hour << "Price:" << price;
            }
            break;
        }
    }
    return prices;
    */
    return QList<double>();
}

PriceRange StockCollection::getPriceRange(const QString& name) const
{

    for(const auto& itr : m_stocks)
    {
        if(itr.m_name == name)
        {
            if(itr.m_prices.isEmpty() == true)
            {
                qDebug() << "Return, empty prices..";
                return PriceRange();
            }

            double sum = 0.0;
            for(const auto& price : itr.m_prices)
            {
                sum += price;
            }
            double average = sum / itr.m_prices.size();

            //Compute ragne as ±25% of average
            double min = average * 0.75;
            min = min < 0 ? 0 : min;
            double max = average * 1.25;

            return PriceRange(min, max, average);
        }
    }
    return PriceRange();
}

void StockCollection::addStockSLOT(const QString& p_name, double p_acqisitionPrice)
{
    if(hasStock(p_name) == false)
    {
        m_stocks.append(Stock { p_name, p_acqisitionPrice } );
        emit stockInitialized(p_name, p_acqisitionPrice);
    }
}

void StockCollection::updateMultiplePricesSLOT(const QString& p_name, std::vector<std::pair<int, double>> p_prices)
{
    //To send vector<pair<value,value>> to QML, it has to be in Qt friendly format
    QList<QVariantMap> qtPrices;
    for(auto itr : p_prices)
    {
        populateStocks(p_name, itr.first, itr.second);
        QVariantMap pair;
        pair["hour"] = itr.first;
        pair["price"] = itr.second;
        qtPrices.append(pair);
    }
    emit multiplePricesUpdated(p_name, qtPrices);
}

void StockCollection::updateSinglePriceSLOT(const QString& p_name, int p_hour, double p_price)
{
    populateStocks(p_name, p_hour, p_price);
    emit singlePriceUpdated(p_name, p_hour, p_price);
    return;
}

void StockCollection::populateStocks(const QString& p_name, int p_hour, double p_prices)
{
    for(auto& itr : m_stocks)
    {
        if(itr.m_name == p_name)
        {
            itr.m_prices[p_hour] = p_prices;
            return;
        }
    }
}

bool StockCollection::hasStock(const QString& name) const
{
    for(auto& itr : m_stocks)
    {
        if(itr.m_name == name) {
            return true;
        }
    }
    return false;
}
