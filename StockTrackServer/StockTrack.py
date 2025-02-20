import yfinance as YahooFinance
import tkinter as Tkinter
import screeninfo as ScreenInfo
import tkinter.ttk as ThemedTkinter
import time
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import (FigureCanvasTkAgg, NavigationToolbar2Tk)
from datetime import datetime
import sqlite3


#CONSTANT VARIABLES
STOCKS_PER_PAGE = 4
DATABASE_NAME = "StockDB.db"
NAME_TABLE = "Stock"
PRICE_TABLE = "Price"

#This is for debug purposes. This should be dynamic eventyally.
stock_container_size = 5

#This is for debuginggin purposes. At this point I have a duplicate copy of database names and symbols. Later on replace this with retreieval from database
SYMBOL_CONTAINER = [
    ("Amazon", "AMZN"), 
    ("Google", "GOOG"), 
    ("Apple", "AAPL") 
    ]

#Constant variable to plot margins
PLOT_MARGIN_LEFT = 0.23
PLOT_MARGIN_RIGHT = 0.95
PLOT_MARGIN_TOP = 0.95
PLOT_MARGIN_BOTTOM = 0.09
PLOT_MARGIN_HSPACE = 0.4

#Conventionally Tkinter "main window" is called "root"
#root = Tkinter.Tk()
monitors = ScreenInfo.get_monitors()

time_axis = ["01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24"]

class StockTrackApplication(Tkinter.Tk) :
    def __init__(self) :
        super().__init__()

        self.page_number = 0
        self.stock_container_size = 4
        self.price_axis = [None] * 24

        #Set up window
        self.SetupWindow()
        self.SetupCanvas()
        self.SetupButtons()
        self.SetupDatabase()        
        self.GetStockData()
        

    #Setup everyting related to window and monitor
    def SetupWindow(self) :

        #See if we have more than one monitor. (This is quite specifically to my setup)
        #Note this does't care if I have more than 2 monitors.
        if(len(monitors) > 1) :
            #If I have second monitor plugger in, use it.
            root_monitor = monitors[1]
        else :
            #Else, use main monitor
            root_monitor = monitors[0]

        #Create window fit for my purposes
        #Create 500x700 window on top left corner of the screen, preferably second monitor.
        self.geometry(f"500x700+{root_monitor.x}+{root_monitor.y}")
        #Set to be borderless
        self.overrideredirect(True)
        #Set title
        self.title("Stock Track")
        #Set resizable to be false
        self.resizable(False, False)
        #Set window transparency
        self.attributes('-alpha', 0.75)
        #Set window to be always on top
        #self.attributes('-topmost', 1)
        #Set the icon for the window.
        self.iconbitmap('./Assets/stocks.ico')

    #Setup Figure, Canvas and Toolbar for drawing (basically a window where to draw)
    def SetupCanvas(self) :
        #Set up figurine and canvas.
        self.figure = Figure(figsize = (8, 5), dpi = 100)
        self.canvas = FigureCanvasTkAgg(self.figure, self)

        self.canvas.get_tk_widget().pack(expand = True, fill = 'both')

        self.toolbar = NavigationToolbar2Tk(self.canvas, self)
        self.toolbar.update()
        self.toolbar.pack_forget()

    #Setup buttons
    def SetupButtons(self) :
        #Create a frame for all buttons, this just hold the relative position for button in the window
        ButtonFrame = Tkinter.Frame(self)
        ButtonFrame.pack(side = Tkinter.BOTTOM, pady = 5, anchor = 'center')

        #Conventional way of creating button object, this doesn't seem to be able to take in parameters
        ThemedTkinter.Button(ButtonFrame, text = "<<", command = self.PrevPage).pack(side = Tkinter.LEFT, padx = 5, pady = 5)
        ThemedTkinter.Button(ButtonFrame, text = "Add Stock", command = AddStock).pack(side = Tkinter.LEFT, padx = 5, pady = 5)
        ThemedTkinter.Button(ButtonFrame, text = ">>", command = self.NextPage).pack(side = Tkinter.LEFT, padx = 5, pady = 5)

    #Setup database if it's not already made
    def SetupDatabase(self) :

        #Create database or connect to it if it already exists.
        databaseConnection = sqlite3.connect(DATABASE_NAME)
        #In order to execute database commands, we need cursor to it.
        databaseCursor = databaseConnection.cursor()

        #Create stocks table to store stock names if it doesn't already exist.
        databaseConnection.execute(f"""CREATE TABLE IF NOT EXISTS {NAME_TABLE}(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        name TEXT,
        symbol TEXT UNIQUE)""")

        databaseConnection.execute(f"""CREATE TABLE IF NOT EXISTS {PRICE_TABLE}(
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        stock_id INTEGER,
        price REAL,
        time TIMESTAMP,
        FOREIGN KEY (stock_id) REFERENCES Stocks(id))""")

        databaseConnection.commit()
        databaseConnection.close()

    #Get the relevant Stock Data from Yahoo API, this is also our "main loop start"
    def GetStockData(self) :
        
        currentTime = time.strftime("%H:%M:%S")
        for singleSymbol in SYMBOL_CONTAINER :
            name, symbol = singleSymbol
            stock_data = YahooFinance.Ticker(symbol)
            currentPrice = stock_data.info.get("currentPrice")
            timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

            print(f"Current Price for {name} : {currentPrice} €")

            #Store data
            self.StoreDataToDatabase(name, currentPrice, timestamp)

        self.UpdatePlotData()
        self.DrawPlotData()
        self.after(3600000, self.GetStockData)


    #Store data retreieved from Yahoo API to database 
    def StoreDataToDatabase(self, stock_name, current_price, current_time) :
        
        #Connect to the database
        databaseConnection = sqlite3.connect(DATABASE_NAME)
        databaseCursor = databaseConnection.cursor()

        #Retrieve stock_id from Stock table
        databaseCursor.execute(f"SELECT id FROM {NAME_TABLE} WHERE name = ?", (stock_name,))
        result = databaseCursor.fetchone()

        if result : 
            stock_id = result[0]
            #Insert into Price table
            databaseCursor.execute(f"INSERT INTO {PRICE_TABLE} (stock_id, price, time) VALUES (?, ?, ?)", (stock_id, current_price, current_time))
            databaseConnection.commit()
        else :
            print(f"Stock {stock_name} not found in database.")

        databaseConnection.close()

    def UpdatePlotData(self):
        
        #Connect to the database where we fetch data from.
        databaseConnection = sqlite3.connect(DATABASE_NAME)
        databaseCursor = databaseConnection.cursor()

        #Today's date in YYYY-MM-DD format for filteing
        today_date = datetime.now().strftime("%Y-%m-%d")

        #For each stock in the symbol container, fetch hourly data for today 
        
        #TODO Alter this loop so that each time y-axis (price axis) is populated, it is also plotted. Otherwise the 
        #plots will interact
        
        for name, symbol in SYMBOL_CONTAINER :
            
            #Fetch stock_id from the Stock table
            databaseCursor.execute(f"SELECT id FROM {NAME_TABLE} WHERE name = ?", (name,))
            result = databaseCursor.fetchone()

            #Check sanity
            if result :
                stock_id = result[0]

                #Query to fetch data points fro the current, day grouping by hour
                databaseCursor.execute(f"""
                    SELECT strftime('%H', time) AS hour, AVG(price) AS avg_price
                    FROM {PRICE_TABLE}
                    WHERE stock_id = ? AND DATE(time) = ?
                    GROUP BY hour
                    """, (stock_id, today_date))

                #Fill y-axis data based on the results
                hourly_data = databaseCursor.fetchall()

                for hour, avg_price in hourly_data :
                    #Discard minutes 
                    hour_index = (int)(hour)
                    self.price_axis[hour_index] = avg_price

            else :
                print(f"Stock {name} not found in database.")

        databaseConnection.close()
        


    #Render / Draw plot data
    def DrawPlotData(self) :
        
        self.figure.clear()

        start = self.page_number * STOCKS_PER_PAGE
        end = min(start + STOCKS_PER_PAGE, stock_container_size)

        for index in range(start, end) :

            plot = self.figure.add_subplot(STOCKS_PER_PAGE, 1, (index - start + 1))
            plot.plot(time_axis, self.price_axis)
            plot.set_title('Stock Name 1', loc = 'left')
            plot.set_xlabel('Time')
            plot.set_ylabel('Price (€)')
            plot.tick_params(axis = 'both', labelsize = 6)
        
        self.figure.subplots_adjust(left = PLOT_MARGIN_LEFT, bottom = PLOT_MARGIN_BOTTOM, right = PLOT_MARGIN_RIGHT, top = PLOT_MARGIN_TOP, hspace = PLOT_MARGIN_HSPACE)
        self.figure.tight_layout(pad = 0.5, h_pad = 0.1)


        self.canvas.draw()


    #Next page button implementation
    def NextPage(self) :
        
        #See how many additional page we can show.
        if self.page_number < (stock_container_size // STOCKS_PER_PAGE) and (stock_container_size % STOCKS_PER_PAGE) > 0:
            self.page_number += 1
            self.DrawPlotData()
        else :
            pass
    #Previous page Button implementation
    def PrevPage(self) :
        if self.page_number > 0 :
            self.page_number -= 1
            self.DrawPlotData()
        else :
            pass

def AddStock() :
    
    databaseConnection = sqlite3.connect(DATABASE_NAME)
    databaseCursor = databaseConnection.cursor()

    databaseConnection.commit()
    databaseConnection.close()



if __name__ == "__main__":
    StockTrack = StockTrackApplication()
    StockTrack.mainloop()

    

