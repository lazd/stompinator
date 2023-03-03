#ifndef __server_h__
#define __server_h__

#define CONFIG_ASYNC_TCP_RUNNING_CORE 1

#include <M5Core2.h>
#include <AsyncTCP.h>
#include <CircularBuffer.h>
#include <ESPAsyncWebServer.h>

#include "Constants.h"
#include "HTML.h"

#define SERVERPORT 80
#define DATAPRECISION 6
#define SERVERUPDATEINTERVAL 450

class WebServer {
private:
  AsyncWebServer *server;
  AsyncWebSocket *ws;

  esp_event_loop_handle_t loopHandle;

  CircularBuffer<float, REWINDSTEPS> buffer;
  unsigned long lastUpdateTime = 0;

  void sendRealTimeData() {
    // Empty the buffer and send it to clients
    String dataString = "u:";
    int i;
    for (i = 0; i < REWINDSTEPS && !buffer.isEmpty(); i++) {
      if (i != 0) {
        dataString += ",";
      }
      dataString += String(buffer.shift(), DATAPRECISION);
    }
    buffer.clear();
    ws->textAll(dataString);
  }

  void sendInstance(InstanceInfo *instanceInfo) {
    String dataString = "i:";
    dataString += instanceInfo->startTime;
    dataString += ",";
    dataString += instanceInfo->duration;
    dataString += ",";
    dataString += instanceInfo->intensity;
    ws->textAll(dataString);
  }

  void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      if (strcmp((char *)data, "calibrate") == 0) {
        esp_event_post_to(this->loopHandle, IMU_EVENT, IMU_CALIBRATE, 0, 0, 0);
      }
    }
  }

  void initWebSocket() {
    ws->onEvent(std::bind(&WebServer::onEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
    server->addHandler(ws);
  }

  static void handleInstanceEvent(void *arg, esp_event_base_t base, int32_t id, void *eventData) {
    WebServer *webServer = (WebServer *)arg;
    InstanceInfo *instanceInfo = (InstanceInfo *)eventData;
    webServer->sendInstance(instanceInfo);
  }

public:
  void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
        break;
    }
  }

  String getDataFiles() {
    String jsonData = String();
    jsonData += '[';

    File root = SD.open("/");
    uint fileCount = 0;
    while (true) {
      File entry = root.openNextFile();

      if (!entry) {
        // no more files
        break;
      }

      if (!entry.isDirectory()) {
        const char *name = entry.name();
        if (strlen(name) >= 3 && strncmp("log", name, 3) == 0) {
          if (fileCount != 0) {
            jsonData += ',';
          }
          jsonData += '{';
          jsonData += "\"name\":";
          jsonData += "\"";
          jsonData += name;
          jsonData += "\"";
          jsonData += ',';
          jsonData += "\"size\":";
          jsonData += entry.size();
          jsonData += '}';
          fileCount++;
        }
      }

      entry.close();
    }
    jsonData += ']';
    return jsonData;
  }

  void start(esp_event_loop_handle_t loopHandle) {
    this->loopHandle = loopHandle;

    esp_event_handler_register_with(loopHandle, WATCHER_EVENT, WATCHER_INSTANCE, handleInstanceEvent, this);

    server = new AsyncWebServer(80);
    ws = new AsyncWebSocket("/ws");
    Serial.printf("Server running on port %d and core %d\n", SERVERPORT, CONFIG_ASYNC_TCP_RUNNING_CORE);

    initWebSocket();

    // Route for root / web page
    server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
      request->send(200, "text/html", index_html);
    });

    server->on("/data.json", HTTP_GET, [this](AsyncWebServerRequest *request) {
      // Root listing
      AsyncWebServerResponse *response = request->beginResponse(200, "application/json", getDataFiles());
      request->send(response);
    });

    server->serveStatic("/data/", SD, "/");

    // Start server
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    server->begin();
  }

  void update(float *data, int size) {
    for (int i = 0; i < size; i++) {
      buffer.push(data[i]);
    }

    if (millis() - lastUpdateTime >= SERVERUPDATEINTERVAL) {
      ws->cleanupClients();
      sendRealTimeData();
      lastUpdateTime = millis();
    }
  }
};

#endif
