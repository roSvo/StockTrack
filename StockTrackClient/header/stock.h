#ifndef STOCKTRACK_STOCK_H
#define STOCKTRACK_STOCK_H

//INCLUDE QT
#include <QString>
#include <QMap>
//INLCUDE STANDARD LIBRARY

//INCLUDE EXTERNAL

//INCLUDE PROJECT


//All relevant data for single stock
class Stock
{
public:
    Stock(const QString p_name = "", double acquisitionPrice = 0.0) :
        m_name(p_name),
        m_acquisitionPrice(acquisitionPrice),
        m_prices{}
    {

    };

    QString m_name;
    double m_acquisitionPrice;
    QMap<int, double> m_prices;
};

#endif //STOCKTRACK_STOCKDATA_H
