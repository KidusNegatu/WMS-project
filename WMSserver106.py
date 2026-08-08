import serial
from serial import Serial
from requests import get
import time
import json
from appwrite.client import Client
from appwrite.id import ID
from appwrite.services.tables_db import TablesDB
import sys
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
        if self.failed:
            return
        self.weatherAPIData()
        if self.failed:
            return
        self.saveForecastDataToServer()
        self.saveDataToServer()

    def serialCom(self):
        try:
            with serial.Serial(self.port, self.baud, timeout=3) as com:
                print("Serial Communicating...")
                serialData = com.readline().decode().strip()
                print("Reading serial...")
                print(serialData)
                time.sleep(2)

            if serialData:
                try:
                    parsedSerial = json.loads(serialData)
                    if isinstance(parsedSerial, dict):
                        self.wholeData.update(parsedSerial)
                except json.JSONDecodeError:
                    print("SERIAL DATA IS NOT VALID JSON!!")
                    self.failed = True

        except serial.SerialException as e:
            print(e)
            self.failed = True
        except KeyboardInterrupt:
            print("Exiting program...")
            self.failed = True
        except Exception as err:
            print("SOMETHING WENT WRONG!!", err)
            self.failed = True
            return 1

    def weatherAPIData(self):
        print("Loading weatherAPI data...")
        apiData = {}
        try:
            res = get(self.currentUrl)
            data = res.json()
            lastUpdate = data["current"]["last_updated"]
            apiData.update({"lastUpdated": lastUpdate})
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
            apiData.update({"windKPH": windkph})
            windDirection = data["current"]["wind_dir"]
            apiData.update({"windDirection": windDirection})
            icon = data["current"]["condition"]["icon"]
            apiData.update({"Icon": icon})
            self.wholeData.update(apiData)
            print(apiData)
            print("Done loading weatherAPI data...")
        except Exception as err:
            print("SOMETHING WENT WRONG WHILE READING WEATHER API DATA!!", err)
            self.failed = True

    def saveDataToServer(self):
        print("Saving data to AppWrite...")
        try:
            rowData = dict(self.wholeData)
            self.wmsTable.create_row(
                database_id="6a73a050001026525006",
                table_id="wms_database",
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
                "maxTemp": data["day"]["maxtemp_c"],
                "minTemp": data["day"]["mintemp_c"],
                "avgTemp": data["day"]["avgtemp_c"],
                "condition": data["day"]["condition"]["text"],
                "conditionIcon": data["day"]["condition"]["icon"],
                "chanceOfRain": data["day"]["daily_chance_of_rain"],
                "avgHumidity": data["day"]["avghumidity"],
                "maxWindKph": data["day"]["maxwind_kph"],
                "sunrise": data["astro"]["sunrise"],
                "sunset": data["astro"]["sunset"]
            }
            self.wmsTable.create_row(
                database_id="6a73a050001026525006",
                table_id="wms_database",
                row_id=ID.unique(),
                data=forecast
            )
            self.wholeData.update(forecast)
            print(aelf.wholeData)

try:
    if __name__ == "__main__":
        try:
            lat = float(input("Latitude: "))
            long = float(input("Longitude: "))
        except Exception:
            print("INVALID INPUT!!")
            sys.exit(1)
        try:
            port = input("Port: ")
        except Exception:
            print("INVALID INPUT!!")
            sys.exit(1)
        while True:
            wms = WMSDataProcessing(port, 9600, lat, long)
            if wms.failed:
                break
            else:
                pass
            time.sleep(2)
    else:
        print("THIS FILE MUST RUN DIRECTLY!!")
except KeyboardInterrupt:
    print("Exiting program...")
