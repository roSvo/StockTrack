//INCLUDE QT
#include <QDebug>
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

QList<double> StockCollection::getPrices(const QString& name) const
{
    QList<double> prices;

    for(const auto& stock : m_stocks)
    {
        if(stock.m_name == name)
        {
            for(unsigned int hour = 0; hour < 24; ++hour)
            {
                prices.append(stock.m_prices.value(hour, 0.0));
            }
            break;
        }
    }
    return prices;
}

PriceRange StockCollection::getPriceRange(const QString& name) const
{

    for(const auto& itr : m_stocks)
    {
        if(itr.m_name == name)
        {
            if(itr.m_prices.isEmpty() == true)
            {
                qDebug() << "Return empty prices..";
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

            qDebug() << "Returning filled prices..";
            return PriceRange(min, max, average);
        }
    }
    return PriceRange();
}

void StockCollection::addStockSLOT(const QString& name, double acqisitionPrice)
{
    if(hasStock(name) == false)
    {
        m_stocks.append(Stock { name, acqisitionPrice } );
        emit stocksChanged();
        qDebug() << "Stock added" << name;
    }
}

void StockCollection::updatePriceSLOT(const QString& name, int hour, double price)
{
    for(auto& itr : m_stocks)
    {
        if(itr.m_name == name)
        {
            itr.m_prices[hour] = price;
            qDebug() << "Updated price for" << name << "at hour" << hour << "to" << price;
            emit priceUpdated(name);
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
