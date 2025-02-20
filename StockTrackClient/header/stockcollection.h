#ifndef STOCKTRACK_STOCKCOLLECTION_H
#define STOCKTRACK_STOCKCOLLECTION_H

//INCLUDE QT
#include <QObject>

//INLCUDE STANDARD LIBRARY

//INCLUDE EXTERNAL

//INCLUDE PROJECT
#include "header/stock.h"

//Scine prices can be very varying we want to sacle the range of each stock's prices
//to match its price.
struct PriceRange
{
    PriceRange(double p_min = 0.0, double p_max = 10.0, double p_average = 5.0)
        : min(p_min)
        , max(p_max)
        , average(p_average)
    {

    }
    double min;
    double max;
    double average;
};


//Collection of all data to be shown to the user
class StockCollection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList stockNames READ getStockNames NOTIFY stocksChanged)

public:
    StockCollection(QObject* p_parent = nullptr);

    //Methods for QML
    QStringList getStockNames() const;
    Q_INVOKABLE QList<double> getPrices(const QString& name) const;
    Q_INVOKABLE PriceRange getPriceRange(const QString& name) const;

public slots:
    void addStock(const QString& name, double acqisitionPrice);
    void updatePrice(const QString& name, int hour, double price);

signals:
    void stocksChanged();
    void priceUpdated();

private:
    QList<Stock> m_stocks;
    bool hasStock(const QString& name) const;

};

#endif //STOCKTRACK_STOCKCOLLECTION_H
