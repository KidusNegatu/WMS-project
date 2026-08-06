#include <DHT.h>
#include <LiquidCrystal.h>
#include <ArduinoJson.h>
const int ldrPin = A0;
const int dhtPin = 10;
const int rainPin = A5;
const int buzzerPin = 9;
const int upBtn = 52;
const int downBtn = 53;
String datas[27];
const int dataLength = 27;
int topItem = 0;
float temp;   
int humidity;
String alert;
String dataPython;

DHT dht(dhtPin, DHT11);
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

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
};
void lcdFun() {
  if (digitalRead(downBtn) == LOW && topItem < dataLength - 2) {
    topItem++;
    delay(50);
  };
  if (digitalRead(upBtn) == LOW && topItem > 0) {
    topItem--;
    delay(50);
  };
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(datas[topItem]);
  lcd.setCursor(0, 1);
  lcd.print(datas[topItem + 1]);
  if(topItem == 3) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alert: ");
    lcd.setCursor(0, 1);
    lcd.print(alert);
  };
};
String labels[2] = {
  "Light: ",
  "Rain: "
};
String values[2];

String weatherAlerts(float temper, float humi, int light, int rain){
  if(temper < 50 && temper >= 35){
    digitalWrite(buzzerPin, HIGH);
    delay(5000);
    digitalWrite(buzzerPin, LOW);
    return "HeatWave&Drought";
  }
  else if(temper < 0 && temper < -20) {
    digitalWrite(buzzerPin, HIGH);
    delay(5000);
    digitalWrite(buzzerPin, LOW);
    return "ColdWave";
  }
  else if(temper > 30 && temper < 45) {
    digitalWrite(buzzerPin, HIGH);
    delay(5000);
    digitalWrite(buzzerPin, LOW);
    return "WildFires";
  }
  else if(temper < 0 && temper >= -3) {
    digitalWrite(buzzerPin, HIGH);
    delay(5000);
    digitalWrite(buzzerPin, LOW);
    return "IceStorm";
  }
  else if(humi > 96 && humi < 100) {
    digitalWrite(buzzerPin, HIGH);
    delay(5000);
    digitalWrite(buzzerPin, LOW);
    return "Fog";
  }
  else if(humi > 80 && humi < 100) {
    digitalWrite(buzzerPin, HIGH);
    delay(5000);
    digitalWrite(buzzerPin, LOW);
    return "Flooding";
  }
  else if(humi > 60 && humi > 100) {
    digitalWrite(buzzerPin, HIGH);
    delay(5000);
    digitalWrite(buzzerPin, LOW);
    return "Thunderstorms";
  }
  else{
    return "None";
  };
};
float pythonTemp;
int pyhtonHumidity;
float pythonLdrValue;
float pythonRainValue;
String pythonAlert;
String pythonTime;
String pythonLastUpdate;
String dayOrNight;
float cloudCover;
float feelsLikeTemp;
float windKph;
float maxWindKph;
float windDirection;
String forcastDate;
float maxTemp;
float minTemp;
float avgTemp;
String condition;
int chanceOfRain;
int avgHumidity;
String sunrise;
String sunset;

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
      };
    };
    while (1);
  };

  if (isnan(ldrValue)) {
    Serial.println("THE PHOTO SENSOR IS NOT WORKING!");
    lcd.clear();
    lcd.print("PHOTO ERROR!!");
    for (int x = 0; x <= 10; x++) {
      if (x % 2 == 0) {
        lcd.blink();
        delay(500);
        lcd.noBlink();
      };
    };
    while (1);
  };

  if (isnan(rainValue)) {
    Serial.println("THE FC37 IS NOT WORKING!!");
    lcd.clear();
    lcd.print("FC37 ERROR!!");
    for (int x = 0; x <= 10; x++) {
      if (x % 2 == 0) {
        lcd.blink();
        delay(500);
        lcd.noBlink();
      };
    };
  };
  values[0] = String(ldrPercentage) + "%";
  values[1] = String(rainPercentage) + "%";
  datas[0] = labels[0] + values[0];
  datas[1] = labels[1] + values[1];
  datas[2] = "Temp: " + String(temp) + (char)223 + "C";
  datas[3] = "Humid: " + String(humidity) + "%";
  datas[4] = alert;
  String dataAsJSON = "{"
           "\"Temp\":" + String(temp) + ","
           "\"Humidity\":" + String(humidity) + ","
           "\"LDRValue\":" + String(ldrPercentage) + ","
           "\"RainValue\":" + String(rainPercentage) + "," 
           "\"Alert\":\"" + alert + "\"" +
       "}";
  Serial.println(dataAsJSON);
  if(Serial.available()){
    dataPython = Serial.readStringUntil('\n');
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, dataPython);
    if(!error) {
      pythonTemp = doc["serialData"]["Temp"];
      pyhtonHumidity = doc["serialData"]["Humidity"];
      pythonLdrValue = doc["serialData"]["LDRValue"];
      pythonRainValue = doc["serialData"]["RainValue"];
      pythonAlert = doc["serialData"]["Alert"];
      pythonTime = doc["apiData"]["time"];
      pythonLastUpdate = doc["apiData"]["lastUpdate"];
      dayOrNight = doc["apiData"]["dayOrNight"];
      cloudCover = doc["apiData"]["cloudCover"];
      feelsLikeTemp = doc["apiData"]["feelsLikeTemp"];
      windKph = doc["apiData"]["windKph"];
      windDirection = doc["apiData"]["windDegree"];
      forcastDate = doc["Forecast"]["date"];
      maxTemp = doc["Forecast"]["maxTemp"];
      minTemp = doc["Forecast"]["minTemp"];
      avgTemp = doc["Forecast"]["avgTemp"];
      condition = doc["Forecast"]["condition"];
      chanceOfRain = doc["Forecast"]["chanceOfRain"];
      avgHumidity = doc["Forecast"]["avgHumidity"];
      maxWindKph = doc["Forecast"]["maxwindKph"];
      sunrise = doc["Forecast"]["sunrise"];
      sunset = doc["Forecast"]["sunset"];

      datas[5]  = "PyTemp: " + String(pythonTemp);
      datas[6]  = "PyHumid: " + String(pyhtonHumidity);
      datas[7]  = "PyLDR: " + String(pythonLdrValue);
      datas[8]  = "PyRain: " + String(pythonRainValue);
      datas[9]  = "PyAlert: " + String(pythonAlert);
      datas[10] = "Time: " + pythonTime;
      datas[11] = "Updated: " + pythonLastUpdate;
      datas[12] = dayOrNight;
      datas[13] = "Cloud: " + String(cloudCover) + "%";
      datas[14] = "Feels: " + String(feelsLikeTemp) + (char)223 + "C";
      datas[15] = "Wind: " + String(windKph) + "kph";
      datas[16] = "MaxWind: " + String(maxWindKph) + "kph";
      datas[17] = "WindDir: " + String(windDirection);
      datas[18] = "Date: " + String(forcastDate);
      datas[19] = "MaxT: " + String(maxTemp) + (char)223 + "C";
      datas[20] = "MinT: " + String(minTemp) + (char)223 + "C";
      datas[21] = "AvgT: " + String(avgTemp) + (char)223 + "C";
      datas[22] = condition;
      datas[23] = "Rain%: " + String(chanceOfRain) + "%";
      datas[24] = "AvgHumid: " + String(avgHumidity) + "%";
      datas[25] = "Sunrise: " + sunrise;
      datas[26] = "Sunset: " + sunset;
    }
  };
  lcdFun();
  delay(300);
};
