import yfinance as YahooFinance
import time
from datetime import datetime, timedelta
import sqlite3
import threading
import socket

import stockprotocol

#Prevent typos. Python doesn't offer constants, but this is intended to be one.
DATABASE_NAME = "StockTrack.db"
NAME_TABLE = "Stock"
PRICE_TABLE = "Price"

#HOST_IP = "192.162.68.105"
HOST_IP = "0.0.0.0"
PORT = 20580


####################################################################################################
########                           SERVER FUNCTIONS                                         ########
####################################################################################################
class StockTrackServerApplication():
    def __init__(self):
        print("Initializing Stock Track Server")
        #Ensure that the database exists.
        databaseConnection = sqlite3.connect(DATABASE_NAME)
        databaseConnection.close()

        self.SetInitialData()

    #This is intended for demo and debug purposes when we don't have outsize access to server yet.
    def SetInitialData(self):
        print("Set-up debug data.")
        databaseConnection = sqlite3.connect(DATABASE_NAME)
        
        #Stock TABLE
        query = f"""CREATE TABLE IF NOT EXISTS {NAME_TABLE}(
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT,
            acquisition_price REAL,
            symbol TEXT
        )"""
        databaseConnection.execute(query)

        #Price TABLE
        query = f"""CREATE TABLE IF NOT EXISTS {PRICE_TABLE}(
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            stock_id INTEGER,
            price REAL,
            time TIMESTAMP,
            FOREIGN KEY (stock_id) REFERENCES {NAME_TABLE}(id)
        )"""
        databaseConnection.execute(query)
        databaseConnection.commit()
        databaseConnection.close()

    def FetchStockData(self):
        
        while True:
            current_time = datetime.now()
            next_update = current_time + timedelta(hours=1)
            print(f"[{current_time}] Fetching socket data..")
            print(f"Next update scheduled for : {next_update.hour}:{next_update.minute}")

            self.GetStockData()
            print("Data fetched and stored succesfully.")

            sleep_time = (next_update - datetime.now()).total_seconds()
            time.sleep(sleep_time)

    #Don't know if this is required yet. We just have to store the data.
    def GetStockData(self):
        
        symbolList = self.GetSymbolList()

        for name, symbol in symbolList:
            try:
                #Insert little delay to avoid rate limits (should not be a problem)
                time.sleep(2) 

                print(f"Fetching data for {symbol}")
                stockData = YahooFinance.Ticker(symbol)

                try:
                    fastInfo = stockData.fast_info
                    #Get latest price from Yahoo
                    currentPrice = float(format(fastInfo.last_price, '.2f'))

                    if currentPrice is None or currentPrice == 0:
                        print(f"No valid price for {symbol}. Skipping..")
                        continue

                    timeStamp = datetime.now().strftime("%Y-%m-%d %H:%M")
                    print(f"Current Price for {name} - {currentPrice:.2f}€")
                    self.StoreDataToDatabase(name, currentPrice, timeStamp)
                
                except Exception as e:
                    print(f"Failed to get price for {symbol}: {e}")

            except Exception as error:
                print(f"Main error for {symbol}: {error}")
                time.sleep(5)

    def StoreDataToDatabase(self, stock_name, current_price, current_time):
        
        #Connect to the database
        databaseConnection = self.GetDatabaseConnection()
        databaseCursor = databaseConnection.cursor()

        #Retrieve stock_id from Stock table
        databaseCursor.execute(f"SELECT id FROM {NAME_TABLE} WHERE name = ?", (stock_name,))
        result = databaseCursor.fetchone()

        if result:
            stock_id = result[0]
            #Insert into Price table
            databaseCursor.execute(f"INSERT INTO {PRICE_TABLE} (stock_id, price, time) VALUES (?, ?, ?)", (stock_id, current_price, current_time))
            databaseConnection.commit()

        else:
            print(f"Stock {stock_name} not found in database")

        databaseConnection.close()        

    
    #Get existing symbols from database, so we know what to update.
    #This will return name and symbol pair
    def GetSymbolList(self) -> list[tuple[str, str]]:

        #Establish database connection, there is no reason to store this to a class variable since our calls will not be very frequent.
        with sqlite3.connect(DATABASE_NAME) as databaseConnection:
            databaseCursor = databaseConnection.cursor()
            databaseCursor.execute(f"SELECT name, symbol FROM {NAME_TABLE}")
            symbols = databaseCursor.fetchall()

        return symbols

    def GetDatabaseConnection(self):
        """Helper function to get a properly configured database connection"""
        connection = sqlite3.connect(DATABASE_NAME)
        connection.execute("PRAGMA timezone = 'localtime'")
        return connection


####################################################################################################
########                           SERVER REQUESTS                                          ########
####################################################################################################


    def HandleRequestCURRENT(self, payload):
        """Retrieve the lastest price for the requested stock."""
        
        if not payload:
            return "Error: Missing payload. Expected 'NAME'."

        stockName = payload.strip()
        databaseConnection = self.GetDatabaseConnection()
        databaseCursor = databaseConnection.cursor()

        query = f"""
        SELECT {PRICE_TABLE}.price, strftime('%H', {PRICE_TABLE}.time, 'localtime') as hour
        FROM {PRICE_TABLE}
        JOIN {NAME_TABLE} ON {PRICE_TABLE}.stock_id = {NAME_TABLE}.id
        WHERE {NAME_TABLE}.name = ?
        AND {PRICE_TABLE}.time >= datetime('now', 'localtime', '-1 hour')
        ORDER BY {PRICE_TABLE}.time DESC
        LIMIT 1;
        """

        databaseCursor.execute(query, (stockName,))
        result = databaseCursor.fetchone()
        databaseConnection.close()

        if result:
            price, hour = result
            return f"{stockName}:{price}:{hour}"
        else:
            return f"Error: No current price found for {stockName}"

    def HandleRequestWEEK(self, payload):
        pass
    def HandleRequestMONTH(self, payload):
        pass
    def HandleRequestQUARTER(self, payload):
        pass
    def HandleRequestYEAR(self, payload):
        pass
    def HandleRequestADD(self, message):
        """Handle adding a new stock to the database"""
        try:
            #Get the first name from the list (when client sends requests there should be only one)
            name = message.stock_names[0]
            symbol = message.stock_symbol
            price = message.acquisition_price

            print(f"DEBUG: Received values - {name} symbol - {symbol} - price {price}")
            print(f"DEUBG: Price type: {type(price)}")

            databaseConnection = self.GetDatabaseConnection()
            databaseCursor = databaseConnection.cursor()

            #Check if stock already exists query.
            query = f"""
            SELECT id
            FROM {NAME_TABLE}
            WHERE name = ? OR symbol = ?
            """
            databaseCursor.execute(query, (name, symbol))
            if databaseCursor.fetchone():
                databaseConnection.close()
                return stockprotocol.format_error("Stock already exists in database")
            
            #Insert new stock.
            query = f"""
            INSERT INTO {NAME_TABLE}
            (name, symbol, acquisition_price)
            VALUES(?, ?, ?)
            """

            databaseCursor.execute(query, (name, symbol, price))
            databaseConnection.commit()
            databaseConnection.close()

            return stockprotocol.format_server_response(
                stockprotocol.MessageType.ADD_STOCK,
                name
            )

        except Exception as e:
            return stockprotocol.format_error(str(e))

    def HandleRequestREMOVE(self, payload):
        pass
    def HandleRequestSTOCKLIST(self):
        """Retrieve all stock names and symbols in the database"""
        #Open database connection
        databaseConnection = self.GetDatabaseConnection()
        databaseCursor = databaseConnection.cursor()

        query = f"""
            SELECT name
            FROM {NAME_TABLE};
        """

        databaseCursor.execute(query)
        results = databaseCursor.fetchall()
        databaseConnection.close()

        if results:

            #Query returns values as tulpes, so first name value would be ("<name1>,<>", "<name2>, <>") and so on. 
            #Initialize empty container to hold only the name vaues without empty tuple pair            
            all_stocks = []
            #Iterate over results
            for name in results:
                #Append name
                all_stocks.append(name[0])
            
            return stockprotocol.format_server_response(
                stockprotocol.MessageType.STOCK_LIST,
                all_stocks
            )
        else:
            return stockprotocol.format_error("No stock found in database")

    def HandleRequestHISTORY(self, message):
        """Retrieve price history for a specific stock for the last 24 hours"""
        if not message.stock_names or len(message.stock_names) == 0:
            return stockprotocol.format_error("Missing stock name")

        #Get stock name from the mssage object
        stockname = message.stock_names[0]
        print(f"Fetchin price history for - {stockname}")

        databaseConnection = self.GetDatabaseConnection()
        databaseCursor = databaseConnection.cursor()

        #Get the latest date for this stock
        query = f"""
            SELECT date({PRICE_TABLE}.time) as latest_date
            FROM {PRICE_TABLE}
            JOIN {NAME_TABLE} ON {PRICE_TABLE}.stock_id = {NAME_TABLE}.id
            WHERE {NAME_TABLE}.name = ?
            ORDER BY {PRICE_TABLE}.time DESC
            LIMIT 1;
        """

        databaseCursor.execute(query, (stockname,))
        latest_date_result = databaseCursor.fetchone()

        if latest_date_result :
            lastest_date = latest_date_result[0]
            print(f"Found latest date : {lastest_date} for {stockname}")

            #Get all price for this date:
            query = f"""
                SELECT {PRICE_TABLE}.price, 
                    strftime('%H', {PRICE_TABLE}.time) as hour
                FROM {PRICE_TABLE}
                JOIN {NAME_TABLE} ON {PRICE_TABLE}.stock_id = {NAME_TABLE}.id
                WHERE {NAME_TABLE}.name = ?
                AND date({PRICE_TABLE}.time) = ?
                ORDER BY {PRICE_TABLE}.time ASC;
            """

        databaseCursor.execute(query, (stockname, lastest_date))
        results = databaseCursor.fetchall()
        databaseConnection.close()

        if results:
            prices = []
            for values in results:
                #Get first column: price
                price = float(values[0])
                #Get second column: hour
                hour = int(values[1])

                #Create tuple and append prices
                price_entry = (hour, price)
                prices.append(price_entry)

            return stockprotocol.format_server_response(
                stockprotocol.MessageType.HISTORY,
                stockname,
                0.0,
                prices
            )

        return stockprotocol.format_error(f"No pirce history foudn for {stockname}")


####################################################################################################
########                           LISTEN CLIENTS                                           ########
####################################################################################################


    def ClientRequest(self, message):
        """Hande the client's request and return the response"""

        #Handle the command
        if message.message_type == stockprotocol.MessageType.ADD_STOCK:
            return self.HandleRequestADD(message)
        elif message.message_type == stockprotocol.MessageType.STOCK_LIST:
            return self.HandleRequestSTOCKLIST()
        elif message.message_type == stockprotocol.MessageType.CURRENT_PRICE:
            return self.HandleRequestCURRENT(message)
        elif message.message_type == stockprotocol.MessageType.HISTORY:
            return self.HandleRequestHISTORY(message)
        else:
            return "Invalid request command."       
    

    def ListenClients(self):

        """Listen for incoming clinet connections and handle them."""
        serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        #Enable address reuse to avoid "Address already in use" errors        
        serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        
        try:
            serverSocket.bind((HOST_IP, PORT))
            serverSocket.listen(5)
            print(f"Server listening on {HOST_IP}:{PORT}")

            while True:
                clientSocket, clientAddress = serverSocket.accept()
                print(f"Connect from {clientAddress}")

                try:
                    #Receive the client's request.
                    requestData = clientSocket.recv(1024).decode("utf-8").strip()
                    print(f"Received request {requestData}")

                    message = stockprotocol.parse_message(requestData)
                    #Handle request, each request is responsible of formatting the response data
                    response = self.ClientRequest(message)
                    #Send down the response
                    clientSocket.sendall(response.encode("utf-8"))

                except Exception as error:
                    print(f"Error hanlding request: {error}")
                    errorResponse = stockprotocol.format_error("Could not handle request.")
                    clientSocket.sendall(errorResponse.encode('utf-8'))

                finally:
                    print(f"Client request handled, closing connection.")
                    clientSocket.close()

        except KeyboardInterrupt:
            print("Shutting down the server...")
        
        finally:
            #Ensure the socket is closed properly
            serverSocket.close()
            print("Server socket closed.")

    def StartServer(self):

        #Start the data fetching thread
        dataFetchThread = threading.Thread(target = self.FetchStockData, daemon = True)
        dataFetchThread.start()

        #Run the clinet listener in the main thread
        self.ListenClients()


if __name__ == "__main__":
    StockTrackServer = StockTrackServerApplication()
    StockTrackServer.StartServer()


