cmake_minimum_required(VERSION 3.16)

project(StockTrackClientProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt6 6.5 REQUIRED COMPONENTS
    Quick
    Widgets
)

add_library(stockprotocol STATIC IMPORTED)
set_target_properties(stockprotocol PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib/stockprotocol.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/header"
)

# Debug message for library properties
get_target_property(LIB_LOCATION stockprotocol IMPORTED_LOCATION)
message(STATUS "Protocol library location: ${LIB_LOCATION}")

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(ApplicationStockTrackClient
    header/tcpconnect.h
    header/stockcollection.h
    header/stock.h
    header/requesthandler.h
    src/tcpconnect.cpp
    src/stockcollection.cpp
    src/stock.cpp
    src/requesthandler.cpp
    src/main.cpp
)

qt_add_qml_module(ApplicationStockTrackClient
    URI StockTrackClient
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/PlotChart.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(ApplicationStockTrackClient PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appStockTrackClient
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(ApplicationStockTrackClient
    PRIVATE
    Qt6::Quick
    Qt6::Widgets
    stockprotocol
)

include(GNUInstallDirs)
install(TARGETS ApplicationStockTrackClient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
