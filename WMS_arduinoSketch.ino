#include <DHT.h>
#include <LiquidCrystal.h>

const int ldrPin = A0;
const int dhtPin = 10;
const int rainPin = A5;
const int buzzerPin = 9;
const int upBtn = 52;
const int downBtn = 53;
String datas[4];
const int dataLength = 4;
int topItem = 0;
float temp;   
float humidity;

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
};
String labels[2] = {
  "Light: ",
  "Rain: "
};
String values[2];

void loop() {
  temp = dht.readTemperature();
  humidity = dht.readHumidity();
  float ldrValue = analogRead(ldrPin);
  int ldrPercentage = map(ldrValue, 0, 1023, 0, 100);
  float rainValue = analogRead(rainPin);
  int rainPercentage = map(rainValue, 0, 1023, 0, 100);
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
  String dataAsJSON = "{"
           "\"Temp\":" + String(temp) + ","
           "\"Humidity\":" + String(humidity) + ","
           "\"LDRValue\":" + String(ldrPercentage) + ","
           "\"LDRValueADC\":" + String(ldrValue) + ","
           "\"RainValue\":" + String(rainPercentage) + "," 
           "\"RainValueADC\":" + String(rainValue) + ","  
       "}";
  Serial.println(dataAsJSON);
  lcdFun();
  delay(3000);
};
