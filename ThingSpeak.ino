#include <Wire.h>

#include <SoftwareSerial.h>

#include <math.h>
#include <stdlib.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#define SSID "OnePlus3" //replace XXXXX by your router SSID
#define PASS "Bibse123" //replace YYYYY by your router password
#define IP "184.106.153.149" // thingspeak.com IP

String GET = "GET /update?key=JYHFERF8V9FK3DOD&field1="; //replace ZZZZZ by your ThingSpeak channel write key
SoftwareSerial monitor(10, 11); //Serial communication to ESP8266 module (RX, TX)

Adafruit_BMP085_Unified BMPSensor = Adafruit_BMP085_Unified(10085);

int ThermistorPin = A0; // eigener Temperatursensor
int TempSensorPin = A4; // KY-013 Temperatursensor
int PhotoSensorPin = A5; // KY-018 Fotowiderstandssensor
int count = 1; // Counter für die Tabelle

int ledPin = 3; // weiße LED

// Thermistor
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

// RGB LED
// Konstanten für die drei analogen Input Pins
const int redPot = 0;
const int greenPot = 1;
const int bluePot = 2;

// Konstanten für die drei RGD Pins
const int redPin = 5;
const int greenPin = 6;
const int bluePin = 9;

// Variablen zum speichern der roten, grünen und blauen Werte
int redVal;
int greenVal;
int blueVal;

// Serielle Ausgabe in 9600 Baud
void setup(void)
{
  Serial.begin(9600);
  monitor.begin(9600);
  Serial.println("   ## Spannung   #   Widerstand    #   Temperatur    #    Temperatur2    #    Luftdruck   #    Temperatur3    #   Meereshöhe ##");

  Serial.println("-------------------------------------------------------------------------------------------------------------------------------");

  // RGD Pins als Output setzen
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Weiße LED Pin als Output setzen 
  pinMode(3, OUTPUT);

  //communication with wifi module
  monitor.flush();
  monitor.println("AT");
  delay(2000);

  if(monitor.find("OK")){
      Serial.println("Communication with ESP8266 module: OK");
    }
    else {
      Serial.println("ESP8266 module ERROR");
    }

  //connect wifi router  
  connectWiFi(); 

  if (!BMPSensor.begin())
  {
    Serial.println("KY-052-Sensor nicht erkannt!");
    Serial.print("Bitte ueberpruefen Sie die Verbindung");
    while (1);
  }
}

// Diese Funktion übersetzt den aufgenommenen analogen Messwert
// in die entsprechende Temperatur in Grad C und gibt diesen aus
double Thermistor(int RawADC)
{
  double Temp;
  Temp = log(10000.0 * ((1024.0 / RawADC - 1)));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Temp = Temp - 273.15;            // Konvertierung von Kelvin in Celsius
  return Temp;
}

void loop()
{
  
  
  // Tabellenausgabe
  if (count < 10)  {
    Serial.print(" ");
  }
  Serial.print(count);
  count += 1;
  Serial.print(" ## ");

  // Das Programm misst den aktuellen Spannungswert am Sensor,
  // berechnet aus diesen und dem bekannten Serienwiderstand den aktuellen
  // Widerstandswert des Sensors und gibt die Ergebnisse auf der serielle Ausgabe aus
  int rawValue = analogRead(PhotoSensorPin);
  float voltage = rawValue * (5.0 / 1023) * 1000;
  float resistance = 10000 * ( voltage / ( 5000.0 - voltage) );

  // Ausgabe von Spannung und Widerstand
  Serial.print(voltage); Serial.print(" mV");
  Serial.print(" #   ");
  Serial.print(resistance); Serial.print(" Ohm");
  Serial.print("  #   ");

  // Helligkeitswert für die weiße LED
  int light = 255;

  // Weiße LED wird dunkler umso weniger Licht der Fotowiderstandssensor misst
  // geht aus bei Dunkelheit, und wird heller umso mehr Licht gemessen wird
  if (resistance > 45000) {
    analogWrite (ledPin, light - 255);
  } else if (resistance > 40000)  {
    analogWrite (ledPin, light - 220);  
  } else if (resistance > 35000)  {
    analogWrite (ledPin, light - 180);
  } else if (resistance > 30000)  {
    analogWrite (ledPin, light - 140);
  } else if (resistance > 25000)  {
    analogWrite (ledPin, light - 100);
  } else if (resistance > 15000)  {
    analogWrite (ledPin, light);
  }


  // Ausgabe der Temperatur
  int readVal = analogRead(TempSensorPin);
  double temp =  Thermistor(readVal);
  Serial.print(temp);
  Serial.print(" Grad C");
  Serial.print("  #    ");

  // Ausgabe der Temperatur (Thermistor)
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  T = T - 273.15;
  Serial.print(T);
  Serial.print(" Grad C");
  Serial.print("   #    ");

  // RGB LED
  // die drei analog Input Pins werden gelesen und ihr Wert wird in der Farbvariable gespeichert
  redVal = analogRead(redPot);
  greenVal = analogRead(greenPot);
  blueVal = analogRead(bluePot);

  redVal = map(redVal, 0, 1023, 0, 255);
  greenVal = map(greenVal, 0, 1023, 0, 255);
  blueVal = map(blueVal, 0, 1023, 0, 255);

  analogWrite(redPin, redVal);
  analogWrite(greenPin, greenVal);
  analogWrite(bluePin, blueVal);

  if (T < 15)  {
    // blau
    digitalWrite (bluePin, HIGH);
    digitalWrite (greenPin, LOW);
    digitalWrite (redPin, LOW);
  } else if (T < 19) {
    // weiß
    digitalWrite (greenPin, HIGH);
  } else if (T < 24) {
    // grün
    digitalWrite (bluePin, LOW);
  } else if (T < 27) {
    // gelb
    digitalWrite (redPin, HIGH);
  } else if (T < 30)  {
    // lila
    digitalWrite (bluePin, HIGH);
    digitalWrite (greenPin, LOW);
    digitalWrite(redPin, HIGH);
  } else  {
    // rot
    digitalWrite (bluePin, LOW);
    digitalWrite (greenPin, LOW);
    digitalWrite(redPin, HIGH);
  }

  // Initialisierung Adafruit BMP Library
  sensors_event_t event;
  BMPSensor.getEvent(&event);

  // Messung wird gestartet, falls Sensor bereit
  if (event.pressure)
  {
    // Messung des Luftdrucks
    Serial.print(event.pressure);
    Serial.print(" hPa");
    Serial.print("  #    ");

    // Messung der aktuellen Temperatur
    float temperature;
    BMPSensor.getTemperature(&temperature);
    Serial.print(temperature);
    Serial.print(" Grad C");
    Serial.print("   #   ");

    // Berechnung der Hoehe ueber dem Meeresspiegel,
    // aus den aufgenommenen Daten (SLP=1013.25 hPa)
    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
    seaLevelPressure = Serial.print(BMPSensor.pressureToAltitude(seaLevelPressure, event.pressure, temperature));
    Serial.print(" m");
    Serial.print("   ");
    Serial.print("##");
  }

  // Fehlermeldung falls Sensor nicht ausgelesen werden kann
  else
  {
    Serial.println("Sensor-Fehler");
  }

  String lightStr = dtostrf(voltage, 4, 1, buffer);
    lightStr.replace(" ","");
    String temp1Str = dtostrf(temp, 4, 1, buffer);
    temp1Str.replace(" ","");
    String temp2Str = dtostrf(T, 4, 1, buffer);
    temp2Str.replace(" ","");
    String pressureStr = dtostrf(event.pressure, 4, 1, buffer);
    pressureStr.replace(" ","");
    String temp3Str = dtostrf(temperature, 4, 1, buffer);
    temp3Str.replace(" ","");
    String seaLevelStr = dtostrf(seaLevelPressure, 4, 1, buffer);
    seaLevelStr.replace(" ","");

    //send data to ThingSpeak
    updateSensors(lightStr, temp1Str, temp2Str, pressureStr, temp3Str, seaLevelStr);
}
//Send data to ThingSpeak
void updateSensors(String lightStr, String temp1Str, String temp2Str, String pressureStr, String temp3Str, String seaLevelStr) {

  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  monitor.println(cmd);
  delay(2000);
 
  cmd = GET;
  cmd += lightStr;
  cmd += "&field2=";
  cmd += temp1Str;
  cmd += "&field3=";
  cmd += temp2Str;
  cmd += "&field4=";
  cmd += pressureStr;
  cmd += "&field5=";
  cmd += temp3Str;
  cmd += "&field6=";
  cmd += seaLevelStr;
  cmd += "\r\n";
  delay(1000);
  int strsize = cmd.length();
  monitor.println("AT+CIPSEND=" + String(strsize));
  delay(2000);
  
  monitor.print(cmd);
  if(monitor.find("OK")){
    Serial.println("Transmission completed with success");
  }else{
    Serial.println("Transmission failed!");
  }
}

void sendDebug(String cmd){
  Serial.print("SEND: ");
  Serial.println(cmd);
  monitor.println(cmd);
} 
 
boolean connectWiFi(){
  Serial.println("Connecting wi-fi...");
  String cmd ="AT+CWMODE=1";
  monitor.println(cmd);
  delay(2000);
  monitor.flush(); //clear buffer
  cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  monitor.println(cmd);
  delay(5000);
  
  if(monitor.find("OK")){
    Serial.println("Connection succeeded!");
    return true;
  }else{
    Serial.println("Connection failed!");
    return false;
  }
   

  Serial.println();
  Serial.println();

  delay(2000);
}

