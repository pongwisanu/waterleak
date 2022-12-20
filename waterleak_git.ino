#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


#define LED_PIN 5
const int WATER_PIN = A0;

int value = 0;
int delaySendNoti = 0;
int delaySendNotiSec = 60;  // x * 5 sec ex. 60 * 5 = 300 sec
int isAlert = 0;

//Line Notify
String token = ""; //*fill required
String serverName = ""; //*fill required

IPAddress local_IP(, , , ); //*fill required
IPAddress gateway(, , , ); //*fill required
IPAddress subnet(, , , ); //*fill required
IPAddress primaryDNS(, , , ); //*fill required
IPAddress secondaryDNS(, , , ); //*fill required

const char* ssid = ""; //*fill required
const char* pass = ""; //*fill required


void alertOffline(int loopnum) {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}

void alertOnline(String auth, String message) {

  if ((WiFi.status() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);
    http.addHeader("Content-Type", "application/json");
        
    String data = "{\"message\": \"" + message + "\"}";

    int httpResponseCode = http.POST(data);

    http.end();
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);

  delay(1000);

  //connect wifi
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  } else {
    Serial.println("STA configure successful");
  }

  Serial.println("");
  Serial.print("Connecting to WiFi");
  Serial.println("...");
  WiFi.begin(ssid, pass);
  int retries = 0;
  while ((WiFi.status() != WL_CONNECTED)) {
    if (retries > 20 || retries == 0) {
      Serial.println("");
      Serial.print("Attemp to Connect Wifi : status - ");
      Serial.println(WiFi.status());
      Serial.println(ssid);
      Serial.println(pass);
      retries = 0;
    }
    retries++;
    delay(500);
    Serial.print(".");
  }
  if (retries > 14) {
    Serial.println(F("WiFi connection FAILED"));
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected!"));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.println(F("Setup ready"));
  }

  delay(1000);
}

void loop() {

  if (delaySendNoti >= delaySendNotiSec) {
    delaySendNoti = 0;  //set delay for send online notification every 60 second
  }

  value = analogRead(WATER_PIN);

  if (value < 900) {
    //water leak in datacenter and notification
    //offline notification
    alertOffline(5);
    isAlert = 1;
    if (delaySendNoti == 0) {
      //send online notification
      String message = "มีน้ำรั่วที่ห้อง Datacenter บริเวณที่ดับเพลิง !!!!";
      alertOnline(token, message);
    }
    delaySendNoti = delaySendNoti + 1;
  } else {
    //reset value if no longer water leak in datacenter
    if (isAlert == 1) {
      String message = "ไม่มีน้ำรั่วที่ห้อง Datacenter แล้ว :) ";
      alertOnline(token, message);
    }
    delaySendNoti = 0;
    isAlert = 0;
  }

  delay(5000);
}
