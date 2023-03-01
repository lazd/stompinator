#ifndef __server_h__
#define __server_h__

#define CONFIG_ASYNC_TCP_RUNNING_CORE 1

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
  // bool ledState;
  AsyncWebServer *server;
  AsyncWebSocket *ws;

  CircularBuffer<float, REWINDSTEPS> buffer;
  unsigned long lastUpdateTime = 0;

  void notifyClients() {
    // Empty the buffer and send it to clients
    String dataString = String();
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

  void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      if (strcmp((char *)data, "toggle") == 0) {
        // ledState = !ledState;
        // notifyClients();
      }
    }
  }

  void initWebSocket() {
    ws->onEvent(std::bind(&WebServer::onEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
    server->addHandler(ws);
  }

  String templateProcessor(const String &var) {
    if (var == "STATE") {
      // if (ledState) {
      //   return "ON";
      // } else {
      //   return "OFF";
      // }
    }
    return String();
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

  String getDataFile(const char *fileName) {
    String fullFileName = String('/');
    fullFileName += fileName;

    String fileContents = String();
    File file = SD.open(fullFileName.c_str(), FILE_READ);
    if (file) {
      while (file.available()) {
        fileContents += (char)file.read();
      }
      file.close();
    }
    return fileContents;
  }

  void start() {
    server = new AsyncWebServer(80);
    ws = new AsyncWebSocket("/ws");
    Serial.printf("Server running on port %d and core %d\n", SERVERPORT, CONFIG_ASYNC_TCP_RUNNING_CORE);

    initWebSocket();

    // Route for root / web page
    server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
      request->send_P(200, "text/html", index_html, std::bind(&WebServer::templateProcessor, this, std::placeholders::_1));
    });

    server->on("/data.json", HTTP_GET, [this](AsyncWebServerRequest *request) {
      // Root listing
      AsyncWebServerResponse *response = request->beginResponse(200, "application/json", getDataFiles());
      response->addHeader("Access-Control-Allow-Origin", "*");
      request->send(response);
    });

    server->on("/data", HTTP_GET, [this](AsyncWebServerRequest *request) {
      if (request->hasParam("download")) {
        // File request
        const char *fileName = request->getParam("download")->value().c_str();

        // Only allow log files and the sessions file
        if ((strlen(fileName) == 18 && strncmp("log-", fileName, 4) == 0) ||
            (strlen(fileName) == 12 && strncmp("sessions.csv", fileName, 12) == 0)) {
          String fileContents = getDataFile(fileName);
          if (fileContents.length() == 0) {
            Serial.printf("404: %s not found\n", fileName);
            request->send(404);
          } else {
            Serial.printf("200: %s requested\n", fileName);

            String contentDisposition = "attachment; filename=\"";
            contentDisposition += fileName;
            contentDisposition += "\"";

            AsyncWebServerResponse *response = request->beginResponse(200, "text/csv", fileContents);
            response->addHeader("Content-Disposition", contentDisposition);
            response->addHeader("Access-Control-Allow-Origin", "*");

            request->send(response);
          }
        } else {
          Serial.printf("403: Forbidden file requested: %s\n", fileName);
          request->send(403);
        }
      }
      else {
        Serial.println("400: /data request missing download parameter");
        request->send(400);
      }
    });

    // Start server
    server->begin();
  }

  void update(float *data, int size) {
    for (int i = 0; i < size; i++) {
      buffer.push(data[i]);
    }

    if (millis() - lastUpdateTime >= SERVERUPDATEINTERVAL) {
      ws->cleanupClients();
      notifyClients();
      lastUpdateTime = millis();
    }
  }
};

#endif
