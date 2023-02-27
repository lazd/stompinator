
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
      COLORMULTIPLIER = 3;

      constructor() {
        this.tickEven = 1;
        this.buffer = [];
        this.drawBuffer = [];
        this.canvas = document.getElementById('canvas');
        this.context = canvas.getContext('2d');
        this.text = document.getElementById('state');
        this.setCanvasSize();
        window.addEventListener('resize', () => {
          this.setCanvasSize();
        });
        this.text.style.display = 'none';
      }

      interpolateColor(n) {
        const nx = Math.max(Math.min(Math.abs(n) * this.COLORMULTIPLIER, 1), 0);
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
        this.context.fillRect(canvas.width - position, canvas.height / 2 - intensity * canvas.height / 2, 2, intensity * canvas.height);
      }

      storeData(data) {
        this.buffer.push(...data);
      }

      update(tickTime) {
        const framesPerTick = 2;

        // Every time we're called, consume X frames, put them in the draw buffer
        this.drawBuffer.unshift(...this.buffer.splice(0, framesPerTick));

        // Draw from the drawbuffer
        const data = this.buffer.slice(0, framesPerTick);
        console.log(`Buffer size: ${this.buffer.length}`);

        this.context.clearRect(0, 0, this.canvas.width, this.canvas.height);
        for (let i = canvas.width - 1; i >= 0; i--) {
          if (this.drawBuffer[i] !== undefined) {
            this.drawLine(i, this.drawBuffer[i]);
          }
        }

        // once the draw buffer exceeds canvas width, delete stuff
        window.requestAnimationFrame((tickTime) => this.update(tickTime));
      }
    }

    let client;
    let ui;
    window.addEventListener('load', () => {
      ui = new UI();
      client = new Client('192.168.1.205');
      client.addEventListener('data', (event) => {
        ui.storeData(event.detail.data);
      });

      setTimeout(() => {
        ui.update();
      }, 1500);
    });

  </script>
</body>

</html>
)rawliteral";