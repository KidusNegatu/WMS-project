#include <DHT.h>
#include <LiquidCrystal.h>
#include <ArduinoJson.h>

const int ldrPin = A0;
const int dhtPin = 10;
const int rainPin = A5;
const int buzzerPin = 9;
const int upBtn = 52;
const int downBtn = 53;

const int dataLength = 27;
String datas[dataLength];
int itemCount = 0;
int topItem = 0;

float temp;
int humidity;
String alert;
String dataPython;

DHT dht(dhtPin, DHT11);
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

String pythonLastUpdate = "N/A";
String dayOrNight       = "N/A";
float cloudCover        = 0;
float feelsLikeTemp     = 0;
float windKph           = 0;
String windDirection    = "N/A";
float maxTemp           = 0;
float minTemp           = 0;
float avgTemp           = 0;
String condition        = "N/A";
int chanceOfRain        = 0;
int avgHumidity         = 0;
float maxWindKph        = 0;
String sunrise          = "N/A";
String sunset           = "N/A";

void setup() {
  pinMode(ldrPin, INPUT);
  pinMode(dhtPin, INPUT);
  pinMode(rainPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(upBtn, INPUT_PULLUP);
  pinMode(downBtn, INPUT_PULLUP);
  Serial.begin(9600);
  dht.begin();
  lcd.begin(16, 2);
  digitalWrite(buzzerPin, HIGH);
  delay(1000);
  digitalWrite(buzzerPin, LOW);
}

void lcdFun() {
  if (digitalRead(downBtn) == LOW && topItem < itemCount - 2) {
    topItem++;
    delay(50);
  }
  if (digitalRead(upBtn) == LOW && topItem > 0) {
    topItem--;
    delay(50);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(datas[topItem]);
  lcd.setCursor(0, 1);
  lcd.print(datas[topItem + 1]);
}

String weatherAlerts(float temper, float humi, int light, int rain) {
  if (temper < 40 && temper >= 35) {
    digitalWrite(buzzerPin, HIGH);
    delay(3000);
    digitalWrite(buzzerPin, LOW);
    return "HeatWave&Drought";
  } else if (temper < 0) {
    digitalWrite(buzzerPin, HIGH);
    delay(3000);
    digitalWrite(buzzerPin, LOW);
    return "ColdWave";
  } else if (temper < 0 && temper >= -3) {
    digitalWrite(buzzerPin, HIGH);
    delay(3000);
    digitalWrite(buzzerPin, LOW);
    return "IceStorm";
  } else if (humi > 96 && humi < 100) {
    digitalWrite(buzzerPin, HIGH);
    delay(3000);
    digitalWrite(buzzerPin, LOW);
    return "DenseFog";
  } else if (humi > 90 && humi < 100) {
    digitalWrite(buzzerPin, HIGH);
    delay(3000);
    digitalWrite(buzzerPin, LOW);
    return "Flooding";
  } else if (humi > 60 && humi > 800) {
    digitalWrite(buzzerPin, HIGH);
    delay(3000);
    digitalWrite(buzzerPin, LOW);
    return "HeavyRain";
  } else {
    return "None";
  }
}

void buildDisplayData(int ldrPercentage, int rainPercentage) {
  int i = 0;
  datas[i++] = "Temp: " + String(temp, 1) + "C";
  datas[i++] = "Humidity: " + String(humidity) + "%";
  datas[i++] = "Light: " + String(ldrPercentage) + "%";
  datas[i++] = "Rain: " + String(rainPercentage) + "%";
  datas[i++] = "Alert: " + alert;
  datas[i++] = "Updated: " + pythonLastUpdate;
  datas[i++] = dayOrNight;
  datas[i++] = "Cloud: " + String(cloudCover, 0) + "%";
  datas[i++] = "Feels: " + String(feelsLikeTemp, 1) + "C";
  datas[i++] = "Wind: " + String(windKph, 1) + "kph";
  datas[i++] = "WindDir: " + windDirection;
  datas[i++] = "MaxTemp: " + String(maxTemp, 1) + "C";
  datas[i++] = "MinTemp: " + String(minTemp, 1) + "C";
  datas[i++] = "AvgTemp: " + String(avgTemp, 1) + "C";
  datas[i++] = "Cond: " + condition;
  datas[i++] = "RainChance: " + String(chanceOfRain) + "%";
  datas[i++] = "AvgHum: " + String(avgHumidity) + "%";
  datas[i++] = "MaxWind: " + String(maxWindKph, 1) + "kph";
  datas[i++] = "Sunrise: " + sunrise;
  datas[i++] = "Sunset: " + sunset;
  itemCount = i;
}

void loop() {
  temp = dht.readTemperature();
  humidity = dht.readHumidity();
  float ldrValue = analogRead(ldrPin);
  int ldrPercentage = map(ldrValue, 0, 1023, 0, 100);
  float rainValue = analogRead(rainPin);
  int rainPercentage = map(rainValue, 0, 1023, 0, 100);
  alert = weatherAlerts(temp, humidity, ldrPercentage, rainPercentage);

  if (isnan(temp) || isnan(humidity)) {
    Serial.println("THE DHT11 IS NOT WORKING!");
    lcd.clear();
    lcd.print("DHT11 ERROR!!");
    for (int x = 0; x <= 10; x++) {
      if (x % 2 == 0) {
        lcd.blink();
        delay(500);
        lcd.noBlink();
      }
    }
    while (1);
  }

  if (isnan(ldrValue)) {
    Serial.println("THE PHOTO SENSOR IS NOT WORKING!");
    lcd.clear();
    lcd.print("PHOTO ERROR!!");
    for (int x = 0; x <= 10; x++) {
      if (x % 2 == 0) {
        lcd.blink();
        delay(500);
        lcd.noBlink();
      }
    }
    while (1);
  }

  if (isnan(rainValue)) {
    Serial.println("THE FC37 IS NOT WORKING!!");
    lcd.clear();
    lcd.print("FC37 ERROR!!");
    for (int x = 0; x <= 10; x++) {
      if (x % 2 == 0) {
        lcd.blink();
        delay(500);
        lcd.noBlink();
      }
    }
  }

  String dataAsJSON = "{"
           "\"Temp\":" + String(temp) + ","
           "\"Humidity\":" + String(humidity) + ","
           "\"LDRValue\":" + String(ldrPercentage) + ","
           "\"RainValue\":" + String(rainPercentage) + ","
           "\"Alert\":\"" + alert + "\"" +
       "}";
  Serial.println(dataAsJSON);

  if (Serial.available()) {
    dataPython = Serial.readStringUntil('\n');
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, dataPython);

    if (!error) {
      pythonLastUpdate = doc["lastUpdated"]   | pythonLastUpdate;
      dayOrNight       = doc["dayOrNight"]    | dayOrNight;
      cloudCover       = doc["cloudCover"]    | cloudCover;
      feelsLikeTemp    = doc["feelsLikeTemp"] | feelsLikeTemp;
      windKph          = doc["windKPH"]       | windKph;
      windDirection    = doc["windDirection"] | windDirection;
      maxTemp          = doc["maxTemp"]       | maxTemp;
      minTemp          = doc["minTemp"]       | minTemp;
      avgTemp          = doc["avgTemp"]       | avgTemp;
      condition        = doc["condition"]     | condition;
      chanceOfRain     = doc["chanceOfRain"]  | chanceOfRain;
      avgHumidity      = doc["avgHumidity"]   | avgHumidity;
      maxWindKph       = doc["maxWindKph"]    | maxWindKph;
      sunrise          = doc["sunrise"]       | sunrise;
      sunset           = doc["sunset"]        | sunset;
    } else {
      Serial.print("JSON parse failed: ");
      Serial.println(error.c_str());
    }
  }

  buildDisplayData(ldrPercentage, rainPercentage);
  lcdFun();
  delay(300);
}
