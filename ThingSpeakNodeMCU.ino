#include <Adafruit_Sensor.h>

#include <Adafruit_BMP085_U.h>

//library esp
#include <ESP8266WiFi.h>
#include <Wire.h>

Adafruit_BMP085_Unified BMPSensor = Adafruit_BMP085_Unified(10085);

// replace with your channel’s thingspeak API key,
String apiKey = "JYHFERF8V9FK3DOD";                                     //fill in the api key from thingspeak
const char* ssid = "WLAN-68CE91";                                  //fill in your wifi name
const char* password = "6719530062532206";                              //fill in your wifi password
const char* server = "api.thingspeak.com";

int TempSensorPin = 0;

WiFiClient client;

void setup() {


  Serial.begin(115200);
  delay(10);

  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.print("..........");
  Serial.println();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

  }

  Serial.println("WiFi connected");
  Serial.println();

}

double Thermistor(int RawADC)
{
  double Temp;
  Temp = log(10000.0 * ((1024.0 / RawADC - 1)));
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp )) * Temp );
  Temp = Temp - 273.15; // Konvertierung von Kelvin in Celsius
  Temp -= 24; 
  return Temp;
}

void loop() {
  int readVal = analogRead(TempSensorPin);
  double temp =  Thermistor(readVal);
  Serial.print(temp);
  Serial.println(" Grad C");

  if (client.connect(server, 80)) { // "184.106.153.149" or api.thingspeak.com
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temp);
    postStr += "\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n\n\n\n\n\n\n");
    client.print(postStr);



  }
  client.stop();


  // thingspeak needs minimum 15 sec delay between updates
  delay(30000);
}


