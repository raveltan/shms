// Author: Ravel Tan <ravel@buatkode.com>
// Copyright 2021 Buatkode

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SPIFFS.h"
#include "DHT.h"
#include "RTClib.h"
#include <Tone32.h>
#include <string.h>
#include "HX711.h"

const int dout = 33;
const int clk = 32;

HX711 scale(dout, clk);

float calibration_factor = -206650;

#define DHTTYPE DHT11
const int dhtPin = 5;
// Init DHT module
DHT dht(dhtPin, DHTTYPE);

const int buzzerPin = 18;

// RTC instance
RTC_DS3231 rtc;

// Init an lcd display
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";

//Variables to save values from HTML form
String ssid;
String pass;
String ip;
int bottleWeight = 0;

// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";

/* IPAddress localIP; */
IPAddress localIP(192, 168, 1, 200);

// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)

// States traccker variables
char lastDHT[16] = "";
char lastTime[16] = "";

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Read File from SPIFFS
String readFile(fs::FS& fs, const char* path) {
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while (file.available()) {
    fileContent = file.readStringUntil('\n');
    break;
  }
  return fileContent;
}

// Write file to SPIFFS
void writeFile(fs::FS& fs, const char* path, const char* message) {
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("- file written");
  } else {
    Serial.println("- frite failed");
  }
}

// Initialize WiFi
bool initWiFi() {
  if (ssid == "") {
    Serial.println("Undefined SSID");
    return false;
  }

  WiFi.mode(WIFI_STA);

  if (!WiFi.config(localIP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
    return false;
  }
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.println("Connecting to WiFi...");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while (WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("Failed to connect.");
      return false;
    }
  }

  Serial.println(WiFi.localIP());
  return true;
}

void putLine(int line, const char* text) {
  lcd.setCursor(0, line);
  lcd.printf(text);
}
void clearLine(int line) {
  putLine(line, "                ");
}

void writeLine(int line, const char* text) {
  clearLine(line);
  putLine(line, text);
}
void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();

  lcd.init();
  lcd.backlight();

  writeLine(0, "Loading Config  ");
  writeLine(1, "SHMS            ");
  tone(buzzerPin, NOTE_C4, 400, 0);
  scale.set_scale();
  scale.tare();

  initSPIFFS();

  pinMode(buzzerPin, OUTPUT);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    writeLine(0, "RTC Error: 404  ");
    while (1) {}
  }
  /* rtc.adjust(DateTime(2021, 12, 29, 10, 57, 0)); */
  // Load values saved in SPIFFS
  ssid = readFile(SPIFFS, ssidPath);
  pass = readFile(SPIFFS, passPath);

  ip = "192.168.1.200";
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);

  // Wait for all sensors to be initialized
  delay(2000);
  scale.set_scale(calibration_factor);
  writeLine(0, "Starting WIFI   ");
  if (initWiFi()) {
    tone(buzzerPin, NOTE_C4, 400, 0);
    writeLine(0, "Loading Values  ");
    writeLine(1, "Place Bottle Now");
    delay(2000);
    for (;;) {
      int weight = scale.get_units() * -1000;
      char result[16];
      sprintf(result, "%d grams", weight);
      writeLine(0, result);
      if (weight > 20) {
        delay(1000);
        int newWeight = scale.get_units() * -1000;
        if (weight != newWeight) continue;
        char data[16];
        sprintf(data, "%d", weight);
        bottleWeight = weight;
        break;
      }
      delay(500);
    }
    Serial.println("Connection successful");
    writeLine(0, "Connected       ");
    for (;;) {
      scale.set_scale(calibration_factor);
      delay(900);
      // TODO: Organize data sending between delay
      // TODO: Implement caching
      int humidity = (int)dht.readHumidity();
      // Read temperature as Celsius (the default)
      int temperature = (int)dht.readTemperature();

      // Check if any reads failed and exit early (to try again).
      if (isnan(humidity) || isnan(temperature)) {
        Serial.println(F("Failed to read from DHT sensor!"));
      } else {
        char result[16];
        sprintf(result, "%dH | %d'", humidity, temperature);
        if (strcmp(result, lastDHT) != 0) writeLine(0, result);
        strncpy(lastDHT, result, 16);
      }
      delay(900);
      DateTime now = rtc.now();

      char timeResult[16];
      // TODO: Move to water monitoring thread?
      sprintf(timeResult, "%02d:%02d | SHMS", now.hour(), now.minute());
      if (strcmp(timeResult, lastTime) != 0) writeLine(1, timeResult);
      strncpy(lastTime, timeResult, 16);
    }
  } else {
    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");
    // NULL sets an open Access Point
    WiFi.softAP("SHMS", NULL);
    writeLine(1, "SHMS Setup      ");

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    writeLine(0, "                ");
    writeLine(0, IP.toString().c_str());

    // Web Server Root URL
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
      request->send(SPIFFS, "/index.html", "text/html");
    });

    server.serveStatic("/", SPIFFS, "/");

    server.on("/", HTTP_POST, [](AsyncWebServerRequest* request) {
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isPost()) {
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(ssid);
            // Write file to save value
            writeFile(SPIFFS, ssidPath, ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(pass);
            // Write file to save value
            writeFile(SPIFFS, passPath, pass.c_str());
          }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      request->send(200, "text/plain", "Complete, please check your screen");
      writeLine(0, "Restarting...   ");
      tone(buzzerPin, NOTE_C4, 400, 0);
      delay(500);
      ESP.restart();
    });
    server.begin();
  }
}

void loop() {
}
