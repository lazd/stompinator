
#include <ESPAsyncWebServer.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
  <title>Stompinator</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <link href="https://fonts.cdnfonts.com/css/v5-prophit" rel="stylesheet">
  <style>
    :root {
      --rem: 16px;

      --background-color: var(--color-void);
      --foreground-color: var(--color-white);
      --font-size: 1rem;
      --font-family: 'V5 Prophit', 'Courier New', Courier, monospace;

      --color-void: rgb(0, 0, 0);
      --color-gray: rgb(157, 157, 157);
      --color-white: rgb(255, 255, 255);
      --color-red: rgb(190, 38, 51);
      --color-meat: rgb(224, 111, 139);
      --color-darkbrown: rgb(73, 60, 43);
      --color-brown: rgb(164, 100, 34);
      --color-orange: rgb(235, 137, 49);
      --color-yellow: rgb(247, 226, 107);
      --color-darkgreen: rgb(47, 72, 78);
      --color-green: rgb(68, 137, 26);
      --color-brightgreen: rgb(102, 224, 58);
      --color-nightblue: rgb(27, 38, 50);
      --color-seablue: rgb(0, 87, 132);
      --color-skyblue: rgb(49, 162, 242);
      --color-cloudblue: rgb(178, 220, 239);
      --color-purple: rgb(179, 53, 233);
      --color-darkpurple: rgb(81, 22, 106);

      --axis-color: var(--color-brown);
      --axis-label-color: var(--color-yellow);

      --tab-background-color: var(--color-void);
      --tab-border-color: var(--color-orange);
      --tab-label-color: var(--color-brown);
      --tab-value-color: var(--color-darkbrown);
      --tab-font-color: var(--color-white);

      --heading-font-size: 2rem;

      --dialog-border-color: var(--color-skyblue);
      --dialog-header-color: var(--color-seablue);
      --dialog-header-font-color: var(--color-white);
    }

    html {
      font-family: var(--font-family);
      font-smooth: never;
      -webkit-font-smoothing: none;

      font-size: var(--rem);
      background-color: var(--background-color);
      color: var(--foreground-color);
    }

    body {
      display: flex;
      flex-direction: column;
      position: fixed;
      inset: 0;
      gap: 0.75rem;
    }

    html,
    body {
      overscroll-behavior-x: none;
    }

    #top {
      display: flex;
      flex-direction: row;
      height: 50%;
      gap: 0.75rem;
    }

    #bottom {
      height: 50%;
    }

    #canvas {
      width: 100%;
      height: 100%;
    }

    #realtimevalue {
      position: absolute;
      left: 0.5rem;
      bottom: 0.25rem;
    }

    #browser {
      display: flex;
      flex-direction: column;
      height: 100%;
    }

    .tabs {
      display: flex;
      flex-direction: row;
      overflow-y: auto;
      flex-shrink: 0;
      flex-wrap: nowrap;
      gap: 1rem;
      margin: 0 0.5rem;
      scroll-behavior: smooth;
    }

    .tabs::-webkit-scrollbar {
      display: none;
    }

    .tab {
      margin: 0.5rem;
      flex-grow: 0;
      display: flex;
      flex-direction: row;
      align-items: center;
      justify-content: center;
      text-decoration: none;
      font-weight: bold;
      color: var(--tab-font-color);
      border: 2px solid var(--tab-border-color);
      background-color: var(--tab-background-color);
      opacity: 0.75;
      transition: all 125ms ease-in-out;
    }

    .tab-value {
      display: block;
      padding: 0.5rem 1rem;
      white-space: nowrap;
      font-size: 1.5rem;
      background: var(--tab-label-color);
    }

    .tab-label {
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 0 0.5rem;
      height: 100%;
      background: var(--tab-value-color);
    }

    .tab:hover,
    .tab:focus-visible {
      z-index: 1;
      opacity: 1;
    }

    .tab:focus-visible {
      outline: 8px ridge var(--color-purple);
    }

    .tab.is-selected {
      opacity: 1;
    }

    #browser-viewer {
      height: 100%;
      flex: 1;
    }

    .dialog {
      height: 100%;
      flex: 1;
      box-sizing: border-box;
      border: 0.25rem solid var(--dialog-border-color);
      display: flex;
      flex-direction: column;
      gap: 0.5rem;
    }

    .dialog-header {
      margin: 0;
      padding: 0.5rem 0.75rem;
      white-space: nowrap;
      background: var(--dialog-header-color);
      color: var(--dialog-header-font-color);
      font-size: var(--heading-font-size);
    }

    .dialog-content {
      display: flex;
      flex-direction: column;
      flex: 1;
      position: relative;
      overflow-x: auto;
    }

    .dialog-content::-webkit-scrollbar {
      display: none;
    }

    /* Graph */
    .graphAxis line,
    .graphAxis path {
      stroke: var(--axis-color);
    }

    .graphAxis text {
      font-family: var(--font-family);
      font-size: var(--font-size);
      fill: var(--axis-label-color);
    }

    /* table */
    table {
      margin: 0 0.75rem;
      margin-bottom: 1rem;
      border-collapse: collapse;
      font-weight: bold;
    }

    thead {
      color: var(--color-purple);
      text-shadow: 1px 1px var(--color-darkpurple);
      font-weight: normal;
      position: sticky;
      top: 0;
      background-color: var(--color-void);
    }

    th,
    td {
      text-align: left;
      white-space: nowrap;
      padding: 0.5rem;
    }

    .hallOfFame {
      flex: 0;
    }

    .trends {
      width: 50%;
    }
  </style>
</head>

<body>
  <div id="top">
    <div class="dialog hallOfFame">
      <h2 class="dialog-header">hall of fame</h2>
      <div class="dialog-content">
        <table>
          <thead>
            <!-- <th>date</th> -->
            <th>time</th>
            <th>power</th>
            <th>length</th>
          </thead>
          <tbody id="hallOfFame">
          </tbody>
        </table>
      </div>
    </div>

    <div class="dialog trends">
      <h2 class="dialog-header">trends</h2>
      <div class="dialog-content">
        <div id="browser">
          <nav class="tabs" id="browser-list"></nav>
          <div id="browser-viewer"></div>
        </div>
      </div>
    </div>
  </div>

  <div id="bottom">
    <div class="dialog">
      <h2 class="dialog-header">live data</h2>
      <div class="dialog-content">
        <span id="realtimevalue"></span>
        <canvas id="canvas"></canvas>
      </div>
    </div>
  </div>

  <script src="https://d3js.org/d3.v4.js"></script>
  <script>
    function fetch(url) {
      return new Promise((resolve, reject) => {
        const req = new XMLHttpRequest();
        req.timeout = 5000;
        req.addEventListener('load', () => {
          if (req.status === 200) {
            resolve(req.responseText);
          }
          else {
            reject(new Error(`Request failed: ${req.status}`));
          }
        });

        req.addEventListener('error', (evt) => {
          reject(new Error('Request failed'));
        });

        req.addEventListener('abort', (evt) => {
          reject(new Error('Request aborted'));
        });

        req.addEventListener('timeout', (evt) => {
          reject(new Error('Request timed out'));
        });

        req.open('GET', url);
        req.send();
      });
    }

    class Client extends EventTarget {
      gateway = window.location.hostname;
      constructor(server) {
        super();
        this.gateway = `ws://${server ?? this.gateway}/ws`;
        this.connect();
      }

      connect() {
        console.log(`Connecting to ${this.gateway}`);

        this.websocket = new WebSocket(this.gateway);
        this.websocket.onopen = (event) => {
          this.onOpen(event);
          clearInterval(this.reconnectInterval);
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
        this.dispatchEvent(new CustomEvent('open'));
      }

      onClose(event) {
        console.log('Connection closed');
        this.dispatchEvent(new CustomEvent('close'));
        this.reconnectInterval = setInterval(() => this.connect(), 2000);
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
      MINBUFFERSIZE = 120;
      TEXTDRAWINTERVAL = 250;
      SEGMENTSIZE = 6;
      SEGMENTSPACING = 2;
      DATAPOINTERPERTICK = 6;
      SKIPFRAMES = 2;

      running = true;
      lastUpdate = 0;
      lastTextDrawTime = 0;
      frameSkip = 0;

      constructor() {
        this.tickEven = 1;
        this.buffer = [];
        this.drawBuffer = [];
        this.canvas = document.getElementById('canvas');
        this.context = canvas.getContext('2d');
        this.text = document.getElementById('realtimevalue');

        this.setCanvasSize();
        window.addEventListener('resize', () => {
          this.setCanvasSize();
        });
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
        this.canvas.height = this.canvas.parentElement.offsetHeight;
      }

      drawSegmentedVUBar(position, intensity) {
        const start = canvas.height / 2;
        const distance = intensity * start;
        const totalSize = this.SEGMENTSIZE + this.SEGMENTSPACING;
        for (let i = start; i > start - distance; i -= totalSize) {
          const colorFactor = (start - i) / start;
          this.context.fillStyle = this.interpolateColor(colorFactor);
          this.context.fillRect(position, i, this.SEGMENTSIZE, this.SEGMENTSIZE);
        }
        for (let i = start + totalSize; i < start + distance; i += totalSize) {
          const colorFactor = (i - start) / start;
          this.context.fillStyle = this.interpolateColor(colorFactor);
          this.context.fillRect(position, i, this.SEGMENTSIZE, this.SEGMENTSIZE);
        }
      }

      storeData(data) {
        this.lastUpdate = window.performance.now();
        this.buffer.push(...data);
      }

      start() {
        this.running = true;
        this.update();
      }

      stop() {
        this.running = false;
      }

      update(time) {
        // Skip frames since we're so retro
        if (this.frameSkip < this.SKIPFRAMES) {
          this.frameSkip++;
          this.raf();
          return;
        }

        this.frameSkip = 0;

        if (!this.running) {
          return;
        }

        if (this.buffer.length >= this.MINBUFFERSIZE) {
          const framesPerTick = this.DATAPOINTERPERTICK;

          // Every time we're called, consume X frames, put them in the draw buffer
          this.drawBuffer.unshift(...this.buffer.splice(0, framesPerTick));

          // Draw from the drawbuffer
          const data = this.buffer.slice(0, framesPerTick);

          // Drop a frame when the buffer gets too big
          if (this.buffer.length > this.MINBUFFERSIZE * 2) {
            this.buffer.shift();
            console.warn('Dropping a frame');
          }

          if (time - this.lastTextDrawTime >= this.TEXTDRAWINTERVAL) {
            this.text.innerText = parseFloat(this.drawBuffer[0]).toFixed(3);
            this.lastTextDrawTime = time;
          }

          this.context.clearRect(0, 0, this.canvas.width, this.canvas.height);

          const itemsToDraw = Math.round(this.canvas.width / (this.SEGMENTSIZE + this.SEGMENTSPACING));
          for (let curItem = 1; curItem <= itemsToDraw; curItem++) {
            let max = 0;

            let drawLocation = canvas.width - curItem * (this.SEGMENTSIZE + this.SEGMENTSPACING);
            let startIndex = curItem * this.DATAPOINTERPERTICK;

            // Draw only what we have
            if (startIndex < 0) {
              break;
            }

            for (let i = startIndex; i < startIndex + this.DATAPOINTERPERTICK; i++) {
              if (this.drawBuffer[i] > max) {
                max = this.drawBuffer[i];
              }
            }

            this.drawSegmentedVUBar(drawLocation, max);
          }
        }
        else {
          console.warn('Buffer underrun');
        }

        if (this.drawBuffer.length > canvas.width + 1) {
          this.drawBuffer.splice(canvas.width - 1);
        }

        this.raf();
      }

      raf() {
        window.requestAnimationFrame((time) => this.update(time));
      }
    }

    class Browser {
      HEADERLENGTH = 24;
      CONTENTLENGTH = 26;
      server = window.location.hostname;
      currentFile = null;
      resizeTimeout;

      getApproximateEntryCount(size) {
        return (size - this.HEADERLENGTH) / this.CONTENTLENGTH;
      }

      constructor(server) {
        this.server = `http://${server ?? this.server}/`;
        this.el = document.getElementById('browser');
        this.list = document.getElementById('browser-list');
        this.graph = document.getElementById('browser-viewer');
        this.hallOfFame = document.getElementById('hallOfFame');

        void this.update();

        this.el.addEventListener('click', (event) => {
          const anchor = event.target.closest('a');
          if (anchor) {
            const file = anchor.getAttribute('href');
            this.drawGraph(file);
            event.preventDefault();
          }
        });

        this.el.addEventListener('focusin', (event) => {
          event.target.scrollIntoView();
        });

        window.addEventListener('resize', () => {
          this.graph.innerHTML = '';
          clearTimeout(this.resizeTimeout);
          this.resizeTimeout = setTimeout(() => this.drawGraph(this.currentFile), 100);
        });

        // Update every minute
        setInterval(() => {
          this.drawGraph(this.currentFile);
        }, 1000 * 60);
      }

      async update() {
        const fileList = await this.fetchFileList();
        if (fileList) {
          this.drawFileList(fileList);
          if (fileList.length) {
            this.drawGraph(fileList[fileList.length - 1].name);
          }
        }
      }

      async fetchFileList() {
        let response;
        try {
          response = await fetch(this.server + 'data.json');
        }
        catch (err) {
          console.error(`Failed to fetch file list: ${err}`);
          return null;
        }

        let fileList;
        try {
          fileList = JSON.parse(response);
        }
        catch (err) {
          console.error(`Failed to fetch file list: ${err}`);
          return null
        }

        return fileList;
      }

      drawFileList(fileList) {
        this.list.innerHTML = '';
        fileList.forEach(file => {
          const epochTime = Date.parse(file.name.replace(/log-(.*?).csv/, '$1'));
          const date = new Date();
          date.setTime(epochTime);
          this.list.innerHTML += `
          <a class="tab" href="${file.name}">
              <span class="tab-value">${date.toDateString()}</span>
              <span class="tab-label">${Math.round(this.getApproximateEntryCount(file.size)).toLocaleString()}</span>
          </a>`;
        });
      }

      shortHour(d) {
        const amPm = d >= 12 ? 'PM' : 'AM';
        return `${(d % 12) || 12}${amPm}`;
      }

      drawGraph(fileName) {
        if (!fileName) {
          return;
        }

        this.currentFile = fileName;
        const date = fileName.replace(/log-(\d{4}-\d{2}-\d{2}).csv/, '$1');

        const anchors = this.list.querySelectorAll('a');
        for (let anchor of anchors) {
          const isTargetAnchor = anchor.getAttribute('href') === fileName;
          anchor.classList[isTargetAnchor ? 'add' : 'remove']('is-selected');
          if (isTargetAnchor) {
            anchor.scrollIntoView();
          }
        }

        var totalWidth = this.graph.offsetWidth;
        var totalHeight = this.graph.offsetHeight;

        var margin = { top: 20, right: 20, bottom: 40, left: 70 },
          width = totalWidth - margin.left - margin.right,
          height = totalHeight - margin.top - margin.bottom;

        this.graph.innerHTML = '';
        var svg = d3.select('#browser-viewer')
          .append('svg')
          .attr('viewBox', `0 0 ${totalWidth} ${totalHeight}`)
          .append('g')
          .attr('transform',
            'translate(' + margin.left + ',' + margin.top + ')');

        const context = this;

        d3.csv(`${this.server}data?download=${fileName}`,
          function (d) {
            return {
              time: d3.timeParse('%H:%M:%S')(`${date} ${d.time}`),
              hour: parseInt(d.time.split(':')[0]),
              intensity: d.intensity,
              duration: d.duration
            }
          },
          function (data) {
            // ignore impossible outliers
            data = data.filter(item => {
              return item.intensity < 3;
            });

            const topTen = data.slice().sort((a, b) => {
              return b.intensity - a.intensity;
            }).slice(0, 10);

            context.hallOfFame.innerHTML = '';
            for (let item of topTen) {
              context.hallOfFame.innerHTML += `
                <tr>
                  <!-- <td>${date}</td> -->
                  <td>${context.shortHour(item.hour)}</td>
                  <td>${parseFloat(item.intensity).toFixed(2)}</td>
                  <td>${item.duration}ms</td>
                </tr>
              `;
            }

            const hours = {};
            data.forEach(item => {
              const hour = hours[item.hour] = hours[item.hour] || {
                hour: item.hour,
                low: 0,
                med: 0,
                high: 0
              };

              if (item.intensity >= 0.1 && item.intensity < 0.15) {
                hour.low++;
              }
              else if (item.intensity >= 0.15 && item.intensity < 0.20) {
                hour.med++;
              }
              else if (item.intensity >= 0.20) {
                hour.high++;
              }
            });

            data = Object.values(hours);
            data.columns = Object.keys(data[0]);

            const maxSteps = data.reduce((p, c) => {
              const total = c.low + c.med + c.high;
              return total > p ? total : p;
            }, 0);

            // List of subgroups = header of the csv files
            var subgroups = data.columns.slice(1);

            // Group by hour
            var groups = d3.map(data, function (d) { return (d.hour) }).keys();

            // Add X axis
            var x = d3.scaleBand()
              .domain(Array.from(Array(24)).map((v, i) => i))
              .range([0, width])
              .padding([0.2]);

            svg.append('g')
              .attr('transform', 'translate(0,' + height + ')')
              .attr('class', 'graphAxis graphAxis-x')
              .call(d3.axisBottom(x).tickSizeOuter(0).tickFormat(context.shortHour));

            // Add Y axis
            var y = d3.scaleLinear()
              .domain([0, maxSteps])
              .range([height, 0]);

            svg.append('g')
              .attr('class', 'graphAxis graphAxis-y')
              .call(d3.axisLeft(y)
                .tickValues(y.ticks()
                  .filter(tick => Number.isInteger(tick)))
                .tickFormat(d3.format('d')));

            // Color palette: one color per subgroup
            var color = d3.scaleOrdinal()
              .domain(subgroups)
              .range(['var(--color-yellow)', 'var(--color-orange)', 'var(--color-red)'])

            // Stack the data per subgroup
            var stackedData = d3.stack()
              .keys(subgroups)
              (data);

            // Show the bars
            svg.append('g')
              .selectAll('g')
              // Enter in the stack data = loop key per key = group per group
              .data(stackedData)
              .enter().append('g')
              .attr('fill', function (d) { return color(d.key); })
              .selectAll('rect')
              // enter a second time = loop subgroup per subgroup to add all rectangles
              .data(function (d) { return d; })
              .enter().append('rect')
              .attr('x', function (d) { return x(d.data.hour); })
              .attr('y', function (d) { return y(d[1]); })
              .attr('height', function (d) { return y(d[0]) - y(d[1]); })
              .attr('width', x.bandwidth());

            // Hide ticks if necessary
            const ticks = document.querySelectorAll('.graphAxis-x .tick');
            const tickPadding = 8;
            let largestTick = 0;
            ticks.forEach(tick => {
              const rect = tick.getBoundingClientRect();
              largestTick = rect.width > largestTick ? rect.width : largestTick
            });

            if (ticks.length) {
              let lastRect = ticks[0].getBoundingClientRect();
              for (let tick of Array.prototype.slice.call(ticks, 1)) {
                const currentRect = tick.getBoundingClientRect();
                const lastTickCenter = lastRect.right - (lastRect.right - lastRect.left) / 2;
                const curTickCenter = currentRect.right - (currentRect.right - currentRect.left) / 2;
                const overlap = (lastTickCenter + largestTick) > curTickCenter;
                if (overlap) {
                  tick.style.display = 'none';
                }
                else {
                  lastRect = currentRect;
                }
              }
            }
          });
      }
    }

    let client;
    let ui;
    let browser;
    window.addEventListener('load', () => {
      ui = new UI();
      client = new Client(window.server);
      client.addEventListener('data', (event) => {
        ui.storeData(event.detail.data);
      });

      client.addEventListener('open', (event) => {
        ui.start();
      });

      client.addEventListener('close', (event) => {
        ui.stop();
      });

      browser = new Browser(window.server);
    });

  </script>
</body>

</html>
)rawliteral";