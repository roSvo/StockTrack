#ifndef STOCKTRACK_PROTOCOL_H
#define STOCKTRACK_PROTOCOL_H

//INCLUDE QT

//INCLUDE STANDARD LIBRARY
#include <string>
#include <vector>

//INCLUDE EXTERNAL

//INCLUDE PROJECT


//What I want is to have overloaded FormatRequest function which takes in

//String message should be formatted as :
//TYPE: MessageType
//NAME: StockName
//SYBMOL: StockSymbol
//ACQ: AcquisitionPrice
//PRICES: Prices

enum class MessageType
{
    ERROR = 0,
    ADD_STOCK = 1,
    STOCK_LIST,
    CURRENT_PRICE,
    HSOTRY
};

namespace StockTrack
{

//Message for user (Client / Server) to utilize easier access to received message
struct Message
{
    Message() :
        m_messageType{MessageType::ERROR},      //Enum defaul value
        m_stockNames{},                         //Empty vector
        m_stockSymbol{},                        //Empty String
        m_acquisitionPrice{0.0},                //Zero initialization
        m_prices{},                             //Empty vector
        m_success{false}                        //Boolean default
    {

    };

    MessageType m_messageType;
    std::vector<std::string> m_stockNames;
    std::string m_stockSymbol;
    double m_acquisitionPrice;
    std::vector<std::pair<int, double>> m_prices;
    bool m_success;
};

class Protocol
{
public:

    //No need for constructor since static class by design.
    Protocol() = delete;
    Protocol(const Protocol&) = delete;
    Protocol& operator=(const Protocol&) = delete;
    ~Protocol() = delete;


    //Format Client Request overload, these cases cover things I'm at this point able to figure out.
    //Example: GetStockList (list of stocks stored in server database)
    static std::string FormatClientRequest(MessageType p_messageType);
    static std::string FormatClientRequest(MessageType p_messageType, const std::string& p_stockName);
    static std::string FormatClientRequest(MessageType p_messageType, const std::string& p_stockName, const std::string& p_stockSymbol);
    //Example: Add Stock -> ADD_STOCK, "Apple" "AAPL" "150.50$"
    static std::string FormatClientRequest(MessageType p_messageType, const std::string& p_stockName, const std::string& p_stockSymbol, double p_acquisitionPrice);

    //Format Response, utilized by server to send client messages which are formatted in standardized fashion
    //Example: INVALID -> this is not an actual request.
    static std::string FormatServerResponse(MessageType p_messageType);
    //Example: STOCK_LIST: "Apple" "Amazon" "Google" sends back all stocks which are listed in database.
    template <typename ... T>
    static std::string FormatServerResponse(MessageType p_messageType, const std::string& p_stockName, T ... p_remainingNames)
    {
        return AddField("TYPE", getMessageTypeAsString(p_messageType)) +
               AddField("NAME", AddMultipleNames(p_stockName, p_remainingNames...));
    }
    //Example: PRICE_HISTORY / CURRENT_PRICE "Apple":"1100""150.50","1200""151.00"
    static std::string FormatServerResponse(MessageType p_messageType, const std::string& p_stockName, double p_acquisitionPrice, std::vector<std::pair<int, double>> p_prices);

    static std::string FormatError(std::string& p_errorMessage);

    //Parse message into MessageType which can be easily accessed by user. (Client / Server)
    static Message ParseMessage(std::string& p_message);

private:

    //Function to add new field into Response / Request, so it's in standard fashion wihout bigger typos
    static std::string AddField(std::string p_field, std::string p_value);

    //Template function to help parameter pack template function to fill in the rest of the names (p_remainingNames)
    template <typename ... T>
    static std::string AddMultipleNames(std::string& p_name, T ... p_remainingNames)
    {
        //Wihtout if constexpr we would need to define function for single addition AddSingleName(p_name){return p_name} and call it.
        if constexpr (sizeof ... (p_remainingNames) == 0)
        {
            return p_name;
        }
        return p_name + "," + AddMultipleNames(p_remainingNames...);
    }

    //Mapper function which gives the string name for MessageTypes
    static std::string getMessageTypeAsString(MessageType p_messageType);

};

}

#endif
