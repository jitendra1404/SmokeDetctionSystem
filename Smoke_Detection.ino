#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <Wire.h>  
#include <BlynkSimpleEsp8266.h>  
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);  
 
// You should get Auth Token in the Blynk App.
char auth[] = "BpCN9LIAAXxWmOe7A6dgX1ihnhJ5rsW0";
String apiKey = "60444Y1V48S6GB8H";
 
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "hostelhouse";
char pass[] = "sum@4370";
const char* server = "api.thingspeak.com";
WiFiClient client;
 
int buzzer = D3; //buzzer
int sensor = A0; //sensor
int led_green = D5; //no leakage indication
int led_red = D6; // leakage indication
 
// Define threshold value. You might need to change it.
int sensor_limit = 100;
 
void setup()
{
lcd.init();

lcd.backlight();
lcd.setCursor(2,1);

pinMode(buzzer, OUTPUT);
pinMode(sensor, INPUT);
pinMode(led_green, OUTPUT);
pinMode(led_red, OUTPUT);
digitalWrite(led_green, LOW);
digitalWrite(led_red, LOW);
Serial.begin(9600);
Blynk.begin(auth, ssid, pass);
WiFi.begin(ssid, pass);

}
 
void loop()
{
int sensor_value = analogRead(sensor);
Serial.print("Gas Level: ");
Serial.println(sensor_value);

Blynk.virtualWrite(V1, sensor_value);
// Checks if it has reached the threshold value
if (sensor_value > sensor_limit)
{
digitalWrite(led_red, HIGH);
digitalWrite(led_green, LOW);
lcd.print("Smoke Detection: ");
lcd.print(sensor_value);
lcd.print("Alert!!");
digitalWrite(buzzer, HIGH);
delay(500);
digitalWrite(buzzer, LOW);
Blynk.notify("Alert: Smoke Detected");
}
else
{
digitalWrite(buzzer, LOW);
digitalWrite(led_red, LOW);
lcd.print("No Smoke: ");

lcd.print(sensor_value);
digitalWrite(led_green, HIGH);
}

if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
{
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(sensor_value);
    postStr += "r\n";
   
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
}
client.stop();

delay(100);
Blynk.run(); // Initiates Blynk
}
