import serial
from serial import Serial
from requests import get
import time
import json
from appwrite.client import Client
from appwrite.id import ID
from appwrite.services.tables_db import TablesDB

class WMSDataProcessing():
    def __init__(self, port, baud, latitude, longitude):
        self.port = port
        self.baud = baud
        self.wholeData = {}
        self.com = None
        self.currentUrl = f"https://api.weatherapi.com/v1/current.json?key=e24ce09a5d8846b2b3c190302260508&q={latitude},{longitude}"
        self.forecastUrl = f"https://api.weatherapi.com/v1/forecast.json?key=e24ce09a5d8846b2b3c190302260508&q={latitude},{longitude}"
        self.failed = False
        self.user = Client()
        self.user.set_endpoint("https://nyc.cloud.appwrite.io/v1")
        self.user.set_key("standard_8d89de02649192f6fe73af233f026cdf01d2ff32c0988756d068937bda334fa477c6c887460eebff4bb6a85753a8761ff8865d43444da4021ba7f96ea5a7496d835d9a715631838c37cb2e32f8db05a0711eec55ca0cc59d42de7cd002568556883635518bd17e7232e782b1df5e673f82d140f9d9171d9cff3836e2cc469a99")
        self.user.set_project("42914291")
        self.wmsTable = TablesDB(self.user)
        self.wmsForecast = {}
        self.serialCom()
        self.weatherAPIData()
        self.saveDataToServer()
        self.saveForecastDataToServer()
        self.saveDataToJSON()
        self.sendDataToArduino()

    def serialCom(self):
        try:
            self.com = Serial(self.port, self.baud, timeout=3)
            print("Serial Communicating...")
            serialData = self.com.readline().decode().strip()
            print("Reading serial...")
            if serialData:
                self.wholeData.update({"serialData",serialData})
            else:
                self.wholeData = {}
        except serial.SerialException as serialError:
            print("ERROR OCCURED WHILE OPENING SERIAL PORT!!", serialError)
            self.failed = True
            return 
        except KeyboardInterrupt:
            print("Exiting program...")
            self.failed = True
        except Exception as err:
            print("SOMETHING WENT WRONG!!", err)
            self.failed = True
            return 1
        finally:
            if self.com is not None:
                self.com.close()

    def weatherAPIData(self):
        print("Loading weatherAPI data...")
        apiData = {}
        try:
            res = get(self.currentUrl)
            data = res.json()
            time = data["location"]["localtime"]
            apiData.update({"time": time})
            lastUpdate = data["current"]["last_updated"]
            apiData.update({"lastUpdate": lastUpdate})
            dayOrNight = data["current"]["is_day"]
            diurnalCycle = None
            if dayOrNight == 0:
                diurnalCycle = "Night time"
            else:
                diurnalCycle = "Day time"
            apiData.update({"dayOrNight": diurnalCycle})
            cloudCover = data["current"]["cloud"]
            apiData.update({"cloudCover": cloudCover})
            feelsLikeTemp = data["current"]["feelslike_c"]
            apiData.update({"feelsLikeTemp": feelsLikeTemp})
            windkph = data["current"]["wind_kph"]
            apiData.update({"windKph": windkph})
            windDegree = data["current"]["wind_degree"]
            apiData.update({"windDegree": windDegree})
            windDirection = data["current"]["wind_dir"]
            apiData.update({"windDirection": windDirection})
            self.wholeData.update({"apiData": apiData})
            # print(apiData)
            # print(self.wholeData)
            print("Done loading weatherAPI data...")
        except Exception as err:
            print("SOMETHING WENT WRONG WHILE READING WEATHER API DATA!!", err)
            self.failed = True

    def saveDataToJSON(self):
        try:
            with open("WMS.json", "r") as wmsFile:
                self.dataJSON = json.load(wmsFile)
            print("Loading data from WMS.json...")
            self.dataJSON.append(self.wholeData)
            print("Done loading data to WMS.json.")
            print("Saving data to WMS.json...")
            with open("WMS.json", "w") as wmsFile:
                json.dump(self.dataJSON, wmsFile, indent=4)
            print("Done saving data to WMS.json.")
        except json.JSONDecodeError as jsonError:
            print("INVALID JSON FORMAT!!", jsonError)
            self.failed = True
            return
        
    def saveDataToServer(self):
        print("Saving data to AppWrite...")
        try:
            rowData = {}
            if "serialData" in self.wholeData:
                rowData["serialData"] = self.wholeData["serialData"]
            if "apiData" in self.wholeData:
                rowData.update(self.wholeData["apiData"])
            self.wmsTable.create_row(
                database_id="6a73a050001026525006",
                table_id="wms_sensors_tabel",
                row_id=ID.unique(),
                data=rowData
            )
        except Exception as err:
            print("SOMETHING WENT WRONG WHILE UPLOADING DATA TO APPWRITE!!", err)
            self.failed = True
        print("Done saving data to AppWrite.")

    def saveForecastDataToServer(self):
        forecastRespond = get(self.forecastUrl)
        forecastData = forecastRespond.json()
        forecastDay = forecastData["forecast"]["forecastday"]
        for data in forecastDay:
            forecast = {
                "date": data["date"],
                "maxTemp": data["day"]["maxtemp_c"],
                "minTemp": data["day"]["mintemp_c"],
                "avgTemp": data["day"]["avgtemp_c"],
                "condition": data["day"]["condition"]["text"],
                "chanceOfRain": data["day"]["daily_chance_of_rain"],
                "avgHumidity": data["day"]["avghumidity"],
                "maxwindKph": data["day"]["maxwind_kph"],
                "sunrise": data["astro"]["sunrise"],
                "sunset": data["astro"]["sunset"]
            }
            self.wmsTable.create_row(
                database_id="6a73a050001026525006",
                table_id="wmsforcastapidata",
                row_id=ID.unique(), 
                data=forecast
            )
            # print(self.wmsForecast)
            self.wholeData.update({"Forecast": forecast})

    def sendDataToArduino(self):
        try:
            print("Sending data to arduino...")
            arduinoData = json.dumps(self.wholeData) + "\n"
            self.com.write(arduinoData.encode("utf-8"))
            print("Done saving data to arduino...")
        except serial.SerialException as serialError:
            print("WMS IS NOT CONNECTED!!", serialError)
        except Exception as err:
            print("SOMETHING WENT WRONG WHILE SENDING DATA TO ARDUINO!!", err)
        print(self.wholeData)

try:
    if __name__ == "__main__":
        while True:
            wms = WMSDataProcessing("COM6", 9600, 9.02497 , 38.74689)
            if wms.failed:
                break
            else:
                pass
            time.sleep(3)
    else:
        print("THIS FILE MUST RUN DIRECTLY!!")
except KeyboardInterrupt:
    print("Exiting program...")
