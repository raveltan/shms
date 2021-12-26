// Author: Ravel Tan <ravel@buatkode.com>
// Copyright 2021 Buatkode

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SPIFFS.h"

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

void writeLine(int line, const char* text) {
  lcd.setCursor(0, line);
  lcd.printf(text);
}


void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("               ");
  lcd.setCursor(0, 1);
  lcd.print("               ");

  writeLine(0, "Loading Config  ");
  writeLine(1, "SHMS            ");
  initSPIFFS();

  // Load values saved in SPIFFS
  ssid = readFile(SPIFFS, ssidPath);
  pass = readFile(SPIFFS, passPath);
  ip = "192.168.1.200";
  Serial.println(ssid);
  Serial.println(pass);
  Serial.println(ip);

  delay(4000);
  writeLine(0, "Starting WIFI   ");
  if (initWiFi()) {
    /* // Route for root / web page */
    /* server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) { */
    /*   request->send(SPIFFS, "/index.html", "text/html", false, processor); */
    /* }); */
    /* server.serveStatic("/", SPIFFS, "/"); */

    /* // Route to set GPIO state to HIGH */
    /* server.on("/on", HTTP_GET, [](AsyncWebServerRequest* request) { */
    /*   digitalWrite(ledPin, HIGH); */
    /*   request->send(SPIFFS, "/index.html", "text/html", false, processor); */
    /* }); */

    /* // Route to set GPIO state to LOW */
    /* server.on("/off", HTTP_GET, [](AsyncWebServerRequest* request) { */
    /*   digitalWrite(ledPin, LOW); */
    /*   request->send(SPIFFS, "/index.html", "text/html", false, processor); */
    /* }); */
    /* server.begin(); */
    Serial.println("Connection successful");
    writeLine(0, "Connected       ");
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
      request->send(200, "text/plain", "Complete, restarting device" + ip);
      delay(3000);
      ESP.restart();
    });
    server.begin();
  }
}

void loop() {
}
