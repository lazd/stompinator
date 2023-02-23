#ifndef __server_h__
#define __server_h__

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "HTML.h"

class WebServer {
private:
  bool ledState;
  AsyncWebServer *server;
  AsyncWebSocket *ws;

  void notifyClients() {
    ws->textAll(String(ledState));
  }

  void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      if (strcmp((char *)data, "toggle") == 0) {
        ledState = !ledState;
        notifyClients();
      }
    }
  }

  void initWebSocket() {
    ws->onEvent(std::bind(&WebServer::onEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
    server->addHandler(ws);
  }

  String templateProcessor(const String &var) {
    if (var == "STATE") {
      if (ledState) {
        return "ON";
      } else {
        return "OFF";
      }
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

  void start() {
    server = new AsyncWebServer(80);
    ws = new AsyncWebSocket("/ws");

    initWebSocket();

    // Route for root / web page
    server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
      request->send_P(200, "text/html", index_html, std::bind(&WebServer::templateProcessor, this, std::placeholders::_1));
    });

    // Start server
    server->begin();
  }

  void update() {
    ws->cleanupClients();
  }
};

#endif
