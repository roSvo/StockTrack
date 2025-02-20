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
    PriceRange priceRange;
    return priceRange;
}

void StockCollection::addStock(const QString& name, double acqisitionPrice)
{
    if(hasStock(name) == false)
    {
        m_stocks.append(Stock { name, acqisitionPrice } );
        emit stocksChanged();
        qDebug() << "Stock added" << name;
    }
}

void StockCollection::updatePrice(const QString& name, int hour, double price)
{

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
