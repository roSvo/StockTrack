#StockTrackProtocol : Intended to stay in terms about the protocol so neither has to "know" the protocol nor copy it in any way
cmake_minimum_required(VERSION 3.16)

project(StockTrackProtocol VERSION 0.1 LANGUAGES CXX)

if(WIN32)
    set(CMAKE_STATIC_LIBRARY_PREFIX "")
    set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
endif()

#1. Static Libaray for client (Windows/c++) code
add_library(stockprotocol STATIC
    src/stockprotocol.cpp
    header/stockprotocol.h
)

#Install commands
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../StockTrackClient")
    #Create directories if they don't exist
    file(MAKE_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/../StockTrackClient/lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/../StockTrackClient/header"
    )

    #Install the library
    install(TARGETS stockprotocol
        ARCHIVE DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../StockTrackClient/lib"
    )

    #Install the header
    install(FILES header/stockprotocol.h
        DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/../StockTrackClient/header/"
    )
endif()

