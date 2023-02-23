
#include <ESPAsyncWebServer.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <title>Stompinator</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {
      font-family: 'Courier New', Courier, monospace;
      font-weight: bold;
      background-color: black;
      color: green;
    }

    #canvas {
      position: fixed;
      inset: 0;
    }
  </style>
</head>

<body>
  <canvas id="canvas"></canvas>
  <span id="state">%STATE%</span>
  <script>
    class Client extends EventTarget {
      constructor(server) {
        super();
        const gateway = `ws://${server ?? window.location.hostname}/ws`;
        console.log(`Connecting to ${gateway}`);

        this.websocket = new WebSocket(gateway);
        this.websocket.onopen = (event) => {
          this.onOpen(event);
        };

        this.websocket.onclose = (event) => {
          this.onClose(event);
        };

        this.websocket.onmessage = (event) => {
          this.onMessage(event);
        };
      }

      onOpen(event) {
        console.log('Connection opened');
      }

      onClose(event) {
        console.log('Connection closed');
        setTimeout(initWebSocket, 2000);
      }

      onMessage(event) {
        this.dispatchEvent(new CustomEvent('data', {
          detail: {
            data: event.data
          }
        }));
      }

      toggle() {
        websocket.send('toggle');
      }
    }

    class UI {
      constructor() {
        this.buffer = [];
        this.canvas = document.getElementById("canvas");
        this.context = canvas.getContext("2d");
        this.text = document.getElementById('state');
        this.setCanvasSize();
        window.addEventListener('resize', () => {
          this.setCanvasSize();
        });
      }

      setCanvasSize() {
        this.canvas.width = window.innerWidth;
        this.canvas.height = window.innerHeight;
      }

      drawLine(position, intensity) {
        this.context.fillStyle = "green";
        this.context.fillRect(canvas.width - position, canvas.height / 2 - intensity * canvas.height, 2, intensity * canvas.height * 2);
      }

      update(data) {
        this.text.innerHTML = data;
        const intensity = Math.abs(data);
        this.buffer.unshift(intensity);

        this.context.clearRect(0, 0, canvas.width, canvas.height);
        for (let i = canvas.width - 1; i >= 0; i--) {
          if (this.buffer[i] !== undefined) {
            this.drawLine(i, this.buffer[i]);
          }
        }
      }
    }

    let client;
    let ui;
    window.addEventListener('load', () => {
      client = new Client('192.168.1.202');
      client.addEventListener('data', (event) => {
        ui.update(event.detail.data);
      });
      ui = new UI();
    });

  </script>
</body>

</html>
)rawliteral";