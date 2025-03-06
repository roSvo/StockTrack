#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../header/stockprotocol.h"

namespace py = pybind11;

PYBIND11_MODULE(stockprotocol, m) {
    // Bind MessageType enum
    py::enum_<MessageType>(m, "MessageType")
        .value("ERROR", MessageType::ERROR)
        .value("ADD_STOCK", MessageType::ADD_STOCK)
        .value("STOCK_LIST", MessageType::STOCK_LIST)
        .value("CURRENT_PRICE", MessageType::CURRENT_PRICE)
        .value("HISTORY", MessageType::HISTORY)
        .export_values();

    // Bind Message struct
    py::class_<StockTrack::Message>(m, "Message")
        .def(py::init<>())
        .def_readwrite("message_type", &StockTrack::Message::m_messageType)
        .def_readwrite("stock_names", &StockTrack::Message::m_stockNames)
        .def_readwrite("stock_symbol", &StockTrack::Message::m_stockSymbol)
        .def_readwrite("acquisition_price", &StockTrack::Message::m_acquisitionPrice)
        .def_readwrite("prices", &StockTrack::Message::m_prices)
        .def_readwrite("success", &StockTrack::Message::m_success);

    // Bind Protocol as a module-level interface instead of a class (since it's static class)
    m.def("format_client_request",
          static_cast<std::string (*)(MessageType)>(&StockTrack::Protocol::FormatClientRequest));
    m.def("format_client_request",
          static_cast<std::string (*)(MessageType, const std::string&)>(&StockTrack::Protocol::FormatClientRequest));
    m.def("format_client_request",
          static_cast<std::string (*)(MessageType, const std::string&, const std::string&)>(&StockTrack::Protocol::FormatClientRequest));
    m.def("format_client_request",
          static_cast<std::string (*)(MessageType, const std::string&, const std::string&, double)>(&StockTrack::Protocol::FormatClientRequest));
    m.def("format_server_response",
          static_cast<std::string (*)(MessageType)>(&StockTrack::Protocol::FormatServerResponse));
    m.def("format_server_response",
          static_cast<std::string (*)(MessageType, const std::vector<std::string>&)>(&StockTrack::Protocol::FormatServerResponse));
    m.def("format_server_response",
          static_cast<std::string (*)(MessageType, const std::string&, std::pair<int, double>)>(&StockTrack::Protocol::FormatServerResponse));
    m.def("format_server_response",
          static_cast<std::string (*)(MessageType, const std::string&, double, std::vector<std::pair<int, double>>)>(&StockTrack::Protocol::FormatServerResponse));
    m.def("parse_message", &StockTrack::Protocol::ParseMessage);
    m.def("format_error", &StockTrack::Protocol::FormatError);
}
