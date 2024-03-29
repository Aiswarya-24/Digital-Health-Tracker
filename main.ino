#define USE_ARDUINO_INTERRUPTS true
#define DEBUG true
#define SSID "********" // Your WiFi SSID
#define PASS "*************" // Your WiFi password
#define IP "184.106.153.149" // Thingspeak IP
#include <SoftwareSerial.h>
#include <Timer.h>
#include <PulseSensorPlayground.h>
Timer t; // Specify template arguments for Timer
PulseSensorPlayground pulseSensor;
String msg = "GET /update?key=U9GKHTX1JKO2ZZ6V";
19
SoftwareSerial espSerial(10, 11); // Renamed from esp8266
// Variables
const int PulseWire = A0;
const int LED13 = 13;
int Threshold = 550;
float myTemp;
int myBPM;
String BPM;
String temp;
int error;
int panic;
int raw_myTemp;
float Voltage;
float tempC;
void setup() {
Serial.begin(9600);
espSerial.begin(115200);
pulseSensor.analogInput(PulseWire);
20
pulseSensor.blinkOnPulse(LED13);
pulseSensor.setThreshold(Threshold);
if (pulseSensor.begin()) {
Serial.println("We created a pulseSensor Object !");
}
Serial.println("AT");
espSerial.println("AT");
delay(3000);
if (espSerial.find("OK")) {
connectWiFi();
}
t.every(10000, getReadings);
t.every(10000, updateInfo);
}
21
void loop() {
panic_button();
start:
error = 0;
t.update();
if (error == 1) {
goto start;
}
delay(4000);
}
void updateInfo() {
String cmd = "AT+CIPSTART=\"TCP\",\"";
cmd += IP;
cmd += "\",80";
Serial.println(cmd);
espSerial.println(cmd);
22
delay(2000);
if (espSerial.find("Error")) {
return;
}
cmd = msg;
cmd += "&field1=";
cmd += BPM;
cmd += "&field2=";
cmd += temp;
cmd += "\r\n";
Serial.print("AT+CIPSEND=");
espSerial.print("AT+CIPSEND=");
Serial.println(cmd.length());
espSerial.println(cmd.length());
if (espSerial.find(">")) {
Serial.print(cmd);
23
espSerial.print(cmd);
} else {
Serial.println("AT+CIPCLOSE");
espSerial.println("AT+CIPCLOSE");
error = 1;
}
}
boolean connectWiFi() {
Serial.println("AT+CWMODE=1");
espSerial.println("AT+CWMODE=1");
delay(2000);
String cmd = "AT+CWJAP=\"";
cmd += SSID;
cmd += "\",\"";
cmd += PASS;
cmd += "\"";
Serial.println(cmd);
24
espSerial.println(cmd);
delay(5000);
if (espSerial.find("OK")) {
return true;
} else {
return false;
}
}
void getReadings() {
raw_myTemp = analogRead(A0); // Corrected pin from A1 to A0
Voltage = (raw_myTemp / 1023.0) * 5000;
tempC = Voltage * 0.1;
myTemp = (tempC * 1.8) + 32;
int myBPM = pulseSensor.getBeatsPerMinute();
if (pulseSensor.sawStartOfBeat()) {
Serial.println(myBPM);
25
}
delay(20);
char buffer1[10];
char buffer2[10];
BPM = dtostrf(myBPM, 4, 1, buffer1);
temp = dtostrf(myTemp, 4, 1, buffer2);
}
void panic_button() {
panic = digitalRead(8);
if (panic == HIGH) {
Serial.println(panic);
String cmd = "AT+CIPSTART=\"TCP\",\"";
cmd += IP;
cmd += "\",80";
Serial.println(cmd);
espSerial.println(cmd);
26
delay(2000);
if (espSerial.find("Error")) {
return;
}
cmd = msg;
cmd += "&field3=";
cmd += panic;
cmd += "\r\n";
Serial.print("AT+CIPSEND=");
espSerial.print("AT+CIPSEND=");
Serial.println(cmd.length());
espSerial.println(cmd.length());
if (espSerial.find(">")) {
Serial.print(cmd);
espSerial.print(cmd);
} else {
27
Serial.println("AT+CIPCLOSE");
espSerial.println("AT+CIPCLOSE");
error = 1;
}
}
}
