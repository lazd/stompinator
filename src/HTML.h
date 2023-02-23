
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
            data: event.data.split(',')
          }
        }));
      }

      toggle() {
        websocket.send('toggle');
      }
    }

    class UI {
      COLOR_FACTOR = 3;

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

      interpolateColor(n) {
        const nx = Math.max(Math.min(Math.abs(n) * this.COLOR_FACTOR, 1), 0);
        let b = 0;
        let g = 0;
        let r = 0;
        if (nx <= 0.5) {  // first, green stays at 100%, red raises to 100%
          g = 1.0;
          r = 2 * nx;
        } else {  // red stays at 100%, green decays
          r = 1.0;
          g = 1.0 - 2 * (nx - 0.5);
        }
        r *= 255;
        g *= 255;
        b *= 255;
        return `rgb(${r}, ${g}, ${b})`;
      }

      setCanvasSize() {
        this.canvas.width = window.innerWidth;
        this.canvas.height = window.innerHeight;
      }

      drawLine(position, intensity) {
        this.context.fillStyle = this.interpolateColor(intensity);
        this.context.fillRect(canvas.width - position, canvas.height / 2 - intensity * canvas.height, 2, intensity * canvas.height * 2);
      }

      update(data) {
        this.text.innerHTML = data[0];
        this.buffer.unshift(...data);
        const intensity = Math.abs(data);

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