//INCLUDE QT

//INCLUDE STANDARD LIBRARY

//INCLUDE EXTERNAL

//INCLUDE PROJECT
#include "../header/stockprotocol.h"

//Functions to generate standardised request -> no more function dependant messages

std::string StockTrack::Protocol::FormatClientRequest(MessageType p_messageType)
{

    return AddField("TYPE", getMessageTypeAsString(p_messageType));
}

std::string StockTrack::Protocol::FormatClientRequest(MessageType p_messageType, const std::string& p_stockName)
{
    return AddField("TYPE", getMessageTypeAsString(p_messageType)) +
           AddField("NAME", p_stockName);
}

std::string StockTrack::Protocol::FormatClientRequest(MessageType p_messageType, const std::string& p_stockName, const std::string& p_stockSymbol)
{
    return AddField("TYPE", getMessageTypeAsString(p_messageType)) +
           AddField("NAME", p_stockName) +
           AddField("SYMBOL", p_stockSymbol);
}

std::string StockTrack::Protocol::FormatClientRequest(MessageType p_messageType, const std::string& p_stockName, const std::string& p_stockSymbol, double p_acquisitionPrice)
{
    return AddField("TYPE", getMessageTypeAsString(p_messageType)) +
           AddField("NAME", p_stockName) +
           AddField("SYMBOL", p_stockSymbol) +
           AddField("ACQ", std::to_string(p_acquisitionPrice));
}

std::string StockTrack::Protocol::FormatServerResponse(MessageType p_messageType)
{
    return AddField("TYPE", getMessageTypeAsString(p_messageType));
}

std::string StockTrack::Protocol::FormatServerResponse(MessageType p_messageType, const std::string& p_stockName, double p_acquisitionPrice, std::vector<std::pair<int, double>> p_prices)
{
    std::string returnValue = AddField("TYPE", getMessageTypeAsString(p_messageType)) +
                              AddField("NAME", p_stockName) +
                              AddField("ACQ", std::to_string(p_acquisitionPrice));

    std::string pricesStr;
    for(const auto& [hour, price] : p_prices)
    {
        if(!pricesStr.empty())
        {
            //Separate hour,price pairs from each other
            pricesStr += ",";
        }
        pricesStr += std::to_string(hour) + ":" + std::to_string(price);
    }

    return returnValue + AddField("PRICES", pricesStr);
}

std::string StockTrack::Protocol::FormatError(std::string& p_errorMessage)
{
    return AddField("TYPE", getMessageTypeAsString(MessageType::ERROR)) +
           AddField("MESSAGE", p_errorMessage);
}

StockTrack::Message StockTrack::Protocol::ParseMessage(std::string& p_message)
{
    StockTrack::Message returnValue;
    size_t position = 0;

    //Set string posisiont to first found separator marker ":"
    while((position = p_message.find(';')) != std::string::npos)
    {
        //Separate field from the whole mssage Field = [TYPE]:[VALUE];
        std::string field = p_message.substr(0, position);

        //Field is separated by : [TYPE]:[VALUE]
        size_t fieldPosition = field.find(':');
        if(fieldPosition == std::string::npos)
        {
            continue;
        }

        //Split field into type and value
        std::string fieldType = field.substr(0, fieldPosition);
        std::string fieldValue = field.substr(fieldPosition + 1);

        //Parse each field
        if(fieldType == "TYPE")
        {
            if(fieldValue == "ADD_STOCK")
            {
                returnValue.m_messageType = MessageType::ADD_STOCK;
            }
            else if(fieldValue == "STOCK_LIST")
            {
                returnValue.m_messageType = MessageType::STOCK_LIST;
            }
            else if(fieldValue == "CURRENT_PRICE")
            {
                returnValue.m_messageType = MessageType::CURRENT_PRICE;
            }
            else if(fieldValue == "HISTORY")
            {
                returnValue.m_messageType = MessageType::HSOTRY;
            }
            else
            {
                returnValue.m_messageType = MessageType::ERROR;
            }
        }
        else if (fieldType == "NAME")
        {
            //Split comma-separated names
            size_t nameStart = 0;
            size_t nameEnd = 0;

            while((nameEnd = fieldValue.find(',', nameStart)) != std::string::npos)
            {
                returnValue.m_stockNames.push_back(fieldValue.substr(nameStart, nameEnd - nameStart));
                nameStart = nameEnd + 1;
            }
            //Add the last name
            returnValue.m_stockNames.push_back(fieldValue.substr(nameStart));
        }
        else if (fieldType == "SYMBOL")
        {
            returnValue.m_stockSymbol = fieldValue;
        }
        else if (fieldType == "ACQ")
        {
            try
            {
               returnValue.m_acquisitionPrice = std::stod(fieldValue);
            }
            catch(...)
            {
                returnValue.m_success = false;
                return returnValue;
            }
        }
        else if(fieldType == "PRICES")
        {
            //Split price pairs
            size_t priceStart = 0;
            size_t priceEnd = 0;

            while((priceEnd = fieldValue.find(',', priceStart)) != std::string::npos)
            {
                std::string pair = fieldValue.substr(priceStart, priceEnd - priceStart);
                size_t separator = pair.find(":");
                if(separator != std::string::npos)
                {
                    try
                    {
                        int hour = std::stoi(pair.substr(0, separator));
                        double price = std::stod(pair.substr(separator + 1));
                        returnValue.m_prices.emplace_back(hour, price);
                    }
                    catch(...)
                    {
                        returnValue.m_success = false;
                        return returnValue;
                    }
                }
                priceStart = priceEnd + 1;
            }
            //Process the last price pair
            std::string lastPrice = fieldValue.substr(priceStart);
            size_t separator = lastPrice.find(":");
            if(separator != std::string::npos)
            {
                try
                {
                    int hour = std::stoi(lastPrice.substr(0, separator));
                    double price = std::stod(lastPrice.substr(separator + 1));
                    returnValue.m_prices.emplace_back(hour, price);
                }
                catch (...)
                {
                    returnValue.m_success = false;
                    return returnValue;
                }
            }
        }
        //Move to the next field
        p_message = p_message.substr(position + 1);
    }

    returnValue.m_success = true;
    return returnValue;
}

std::string StockTrack::Protocol::AddField(std::string p_field, std::string p_value)
{
    return p_field + ":" + p_value + ";";
}

std::string StockTrack::Protocol::getMessageTypeAsString(MessageType p_messageType)
{
    switch(p_messageType)
    {
    case MessageType::ERROR:
        return "ERROR";
    case MessageType::ADD_STOCK:
        return "ADD_STOCK";
    case MessageType::STOCK_LIST:
        return "STOCK_LIST";
    case MessageType::CURRENT_PRICE:
        return "CURRENT_PRICE";
    case MessageType::HSOTRY:
        return "HISTORY";
    default:
        return "NO_REQUEST";

    }
}

