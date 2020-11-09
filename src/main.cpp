#include <Arduino.h>
#include <WiFi.h>

#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>
#include <DuetStatus.h>
#include <ArduinoOTA.h>
#include <jled.h>

#define HOST_NAME "duetled-esp32"

const char* ssid     = "Wonderland";
const char* password = "password";

const String duetHost = "http://192.168.68.125";
const int    duetPort = 80;
const String duetPassword = "";
const String duetApiUrl = duetHost + "/rr_status?password=" + duetPassword + "&type=3";

const uint32_t pollIntervalMs = 2000;
unsigned long time_now = 0;

const size_t pollObjectCapacity = JSON_ARRAY_SIZE(5) 
  + 7*JSON_ARRAY_SIZE(1) 
  + 4*JSON_ARRAY_SIZE(2) 
  + 3*JSON_ARRAY_SIZE(3) 
  + 3*JSON_OBJECT_SIZE(2) 
  + JSON_OBJECT_SIZE(3) 
  + 4*JSON_OBJECT_SIZE(5) 
  + JSON_OBJECT_SIZE(19) 
  + 500;

DynamicJsonDocument doc(pollObjectCapacity);

#include "RemoteDebug.h"
RemoteDebug Debug;

const uint8_t ledBrightness = 200;
auto LEDR = JLed(15).MaxBrightness(ledBrightness).On();
auto LEDG = JLed(17).MaxBrightness(ledBrightness).On();
auto LEDB = JLed(16).MaxBrightness(ledBrightness).On();


void onStatusChanged(DuetState duetState, DuetState prevState) {
  debugD("Status changed to: %s", DuetStatus::getStatusAsString(duetState).c_str());
  debugD("Status changed from: %s", DuetStatus::getStatusAsString(prevState).c_str());

  if (duetState == DuetState::IDLE) {
    LEDR.On();
    LEDG.Set(130);
    LEDB.Set(20);
  }

  if (duetState == DuetState::PROCESSING) {
    LEDR.On();
    LEDG.On();
    LEDB.On();
  }

  if (duetState == DuetState::IDLE && prevState == DuetState::PROCESSING) {
    LEDR.Off();
    LEDG.On();
    LEDB.Off();
  }
}

WiFiClient client;
DuetStatus duetStatus = DuetStatus(&onStatusChanged);

void setupArduinoOTA() {
  ArduinoOTA.setHostname(HOST_NAME);

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

void pollApi() {
  HTTPClient http;

  debugD("Connecting to %s", duetApiUrl.c_str());

  if (http.begin(client, duetApiUrl)) {
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      DeserializationError error = deserializeJson(doc, payload);

      if (error) {
        debugE("deserializeJson() failed: %s", error.c_str());
        return;
      }

      const char* statusCode = doc["status"];
      debugD("Duet Status: %s", statusCode);
      duetStatus.setStatus(statusCode);
    } else {
      debugE("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  debugI("Connecting to '%s'", ssid);

  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    debugE("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  if (!MDNS.begin(HOST_NAME)) {
    debugE("Error setting up MDNS responder!");
    while(1){
        delay(1000);
    }
  }

  setupArduinoOTA();

  Debug.begin(HOST_NAME); // Initialize the WiFi server
  Debug.setResetCmdEnabled(true); // Enable the reset command
	Debug.showColors(true); // Colors
  Debug.setSerialEnabled(true);

  debugI("WiFi connected");
  debugI("IP address: %s", WiFi.localIP().toString().c_str());
}

void loop() {
  ArduinoOTA.handle();

  if (millis() > time_now + pollIntervalMs) {
    time_now = millis();
    pollApi();
  }

  Debug.handle();

  LEDR.Update();
  LEDG.Update();
  LEDB.Update();
  yield();
}