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
private:
    Q_GADGET
public:
    Q_PROPERTY(double min READ getMin CONSTANT)
    Q_PROPERTY(double max READ getMax CONSTANT)
    Q_PROPERTY(double avg READ getAvg CONSTANT)

public:
    PriceRange(double p_min = 0.0, double p_max = 10.0, double p_average = 5.0)
        : min(p_min)
        , max(p_max)
        , average(p_average)
    {

    }

    double getMin() const { return min; }
    double getMax() const { return max; }
    double getAvg() const { return average; }

private:
    double min;
    double max;
    double average;
};


//Collection of all data to be shown to the user
class StockCollection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList stockNames READ getStockNames NOTIFY pageChanged)

public:
    StockCollection(QObject* p_parent = nullptr);

    //Methods for QML
    QStringList getStockNames() const;
    Q_INVOKABLE QList<double> getPrices(const QString& p_name) const;
    Q_INVOKABLE PriceRange getPriceRange(const QString& p_name) const;

public slots:
    void addStockSLOT(const QString& p_name, double p_acqisitionPrice);
    void updateMultiplePricesSLOT(const QString& p_name, std::vector<std::pair<int, double>> p_prices);
    void updateSinglePriceSLOT(const QString& p_name, int p_hour, double p_price);

signals:
    void pageChanged(int p_startIndex, int p_endIndex);
    void stockInitialized(QString p_name, double p_acquisitionPrice);
    void multiplePricesUpdated(QString p_name, QList<QVariantMap> p_prices);
    void singlePriceUpdated(QString p_name, int p_hour, double p_price);

private:

    void populateStocks(const QString& p_name, int p_hour, double p_price);
    bool hasStock(const QString& p_name) const;

    QList<Stock> m_stocks;

};

#endif //STOCKTRACK_STOCKCOLLECTION_H
