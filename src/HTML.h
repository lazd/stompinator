
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
      --component-height: 2rem;
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
      --color-lightpurple: rgb(179, 53, 233);
      --color-purple: rgb(147, 47, 189);
      --color-darkpurple: rgb(109, 30, 143);

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

      --button-color: var(--color-darkpurple);
      --button-color-hover: var(--color-purple);
      --button-border-color: var(--color-purple);
      --button-border-color-hover: var(--color-lightpurple);
      --button-border-width: 0.125rem;
      --button-press-distance: 0.125rem;
      --button-padding: 0.5rem;
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
      transition: transform 20ms linear;
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
      font-size: 2rem;
      opacity: 1;
      transition: color 250ms linear;
    }

    #realtimevalue.fadeOut {
      transition: opacity 3s linear, color 250ms linear;
      opacity: 0;
    }

    #browser {
      display: flex;
      flex-direction: column;
      height: 100%;
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
    }

    .dialog-header {
      display: flex;
      flex-direction: row;
      align-items: center;
      gap: 1rem;
      margin: 0;
      white-space: nowrap;
      background: var(--dialog-header-color);
      color: var(--dialog-header-font-color);
      font-size: var(--heading-font-size);
      height: calc(var(--component-height) + 0.5rem);
      padding: 0 0.25rem;
    }

    .dialog-header h2 {
      flex: 1;
      margin: 0 0.5rem;
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
    svg {
      display: block;
    }

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
      color: var(--color-lightpurple);
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

    /* button */
    select,
    button {
      appearance: none;
      background-color: var(--button-color);
      border: var(--button-border-width) solid var(--button-border-color);
      margin: 0;
      padding: 0 var(--button-padding);

      font-family: var(--font-family);
      font-size: var(--font-size);
      font-weight: bold;
      color: var(--foreground-color);
      height: var(--component-height);

      transition: all 75ms linear;
    }

    select:focus,
    button:focus {
      outline: none;
    }

    select:focus-visible,
    button:focus-visible {
      outline: 0.25rem ridge var(--color-orange);
      animation: outline-throb 500ms ease-in-out infinite alternate;
    }

    select:hover,
    button:hover {
      cursor: pointer;
      --button-border-color: var(--button-border-color-hover);
      --button-color: var(--button-color-hover);
    }

    select:active,
    button:active {
      box-shadow: var(--button-press-distance) var(--button-press-distance) var(--color-void) inset;
      background: var(--button-color);
    }

    button:active {
      padding-top: var(--button-press-distance);
      padding-left: calc(var(--button-padding) + var(--button-press-distance));
      padding-right: calc(var(--button-padding) - var(--button-press-distance));
    }

    select {
      --select-box-size: 1.75rem;
      --select-triangle-size: 0.5rem;
      --select-triangle-x-position: 0.3625rem;
      --select-triangle-y-position: 0.625rem;
      padding-right: 2rem;
      background-image:
        linear-gradient(45deg, transparent 50%, var(--color-void) 50%),
        linear-gradient(135deg, var(--color-void) 50%, transparent 50%),
        linear-gradient(to right, var(--button-border-color), var(--button-border-color));

      background-position:
        calc(100% - var(--select-triangle-size) - var(--select-triangle-x-position)) var(--select-triangle-y-position),
        calc(100% - var(--select-triangle-x-position)) var(--select-triangle-y-position),
        100% 0;

      background-size:
        var(--select-triangle-size) var(--select-triangle-size),
        var(--select-triangle-size) var(--select-triangle-size),
        var(--select-box-size) var(--select-box-size);

      background-repeat: no-repeat;
    }

    @keyframes outline-throb {
      0% {
        outline-offset: 0.125rem;
      }

      100% {
        outline-offset: 0.25rem;
      }
    }

    @media only screen and (max-width: 700px) {
      :root {
        --heading-font-size: 1.5rem;
        --rem: 14px;
      }

      .hallOfFame {
        display: none;
      }
    }
  </style>
</head>

<body>
  <div id="top">
    <div class="dialog hallOfFame">
      <div class="dialog-header">
        <h2>hall of fame</h2>
      </div>
      <div class="dialog-content">
        <table>
          <thead>
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
      <div class="dialog-header">
        <h2>trends</h2><select id="browser-picker"></select>
      </div>
      <div class="dialog-content">
        <div id="browser">
          <div id="browser-viewer"></div>
        </div>
      </div>
    </div>
  </div>

  <div id="bottom">
    <div class="dialog liveData">
      <div class="dialog-header">
        <h2>live data</h2><button
          onclick="this.dispatchEvent(new Event('calibrate', { bubbles: true }))">calibrate</button>
      </div>
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
        const [type, dataString] = event.data.split(':');
        const data = dataString.split(',');
        this.dispatchEvent(new CustomEvent('data', {
          detail: {
            type,
            data
          }
        }));
      }

      sendMessage(message) {
        this.websocket.send(message);
      }
    }

    class UI {
      COLORMULTIPLIER = 3;
      MINBUFFERSIZE = 6;
      TEXTDRAWINTERVAL = 250;
      SEGMENTSIZE = 6;
      SEGMENTSPACING = 2;
      DATAPOINTERPERTICK = 1;
      SKIPFRAMES = 2;
      SCREENSHAKEFACTOR = 10;

      running = true;
      lastUpdate = 0;
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
        this.canvas.height = 0; // reset height to get a valid measurement
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

      updateText(intensity) {
        this.text.classList.remove('fadeOut');
        this.text.innerText = parseFloat(intensity).toFixed(2);
        this.text.style.color = this.interpolateColor(intensity);
        clearTimeout(this.textTimeout);
        this.textTimeout = setTimeout(() => {
          this.text.classList.add('fadeOut');
        }, 1000);
      }

      shakeScreen(intensity) {
        if (intensity < 0.1) {
          document.body.style.transform = '';
          return;
        }
        let c = intensity * this.SCREENSHAKEFACTOR;
        let beta = Math.random() * 90;
        let a = c * Math.cos(beta);
        let b = c * Math.sin(beta);
        document.body.style.transform = `translate3d(${a}px, ${b}px, ${c / 2}px)`;
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
          if (this.buffer.length > this.MINBUFFERSIZE * 2.5) {
            // If we've ended up in a state where we've got a gigantic queue of useless data, just start over
            if (this.buffer.length > this.MINBUFFERSIZE * 10) {
              console.warn('Buffer is enormous, starting fresh a frame');
              this.buffer.length = 0;
              this.raf();
              return;
            }

            this.buffer.shift();
            console.info('Dropping a frame');
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

            if (curItem === 1) {
              this.shakeScreen(max);
            }
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
      currentData = null;
      currentDate = null;

      getApproximateEntryCount(size) {
        return (size - this.HEADERLENGTH) / this.CONTENTLENGTH;
      }

      constructor(server) {
        this.server = `http://${server ?? this.server}/`;
        this.el = document.getElementById('browser');
        this.picker = document.getElementById('browser-picker');
        this.graph = document.getElementById('browser-viewer');
        this.hallOfFame = document.getElementById('hallOfFame');

        void this.update();

        this.picker.addEventListener('change', (event) => {
          const select = event.target.closest('select');
          if (select) {
            this.fetchData(select.value);
            select.blur();
          }
        });

        this.el.addEventListener('focusin', (event) => {
          event.target.scrollIntoView();
        });

        window.addEventListener('resize', () => {
          if (this.currentData) {
            this.graph.innerHTML = '';
            this.drawGraph();
          }
        });

        // Update every minute
        setInterval(() => {
          this.fetchData(this.currentFile);
        }, 1000 * 60);
      }

      async update() {
        const fileList = await this.fetchFileList();
        if (fileList) {
          this.drawFilePicker(fileList);
          if (fileList.length) {
            const file = fileList[fileList.length - 1];
            if (file.size < 1024 * 1024) {
              this.fetchData(file.name);
            }
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

      drawFilePicker(fileList) {
        this.picker.innerHTML = '';
        fileList.slice().reverse().forEach(file => {
          const dateString = file.name.replace(/log-(.*?).csv/, '$1');
          const [YYYY, MM, DD] = dateString.split('-');
          const date = new Date(YYYY, MM - 1, DD);
          this.picker.innerHTML += `
          <option value="${file.name}">${date.toDateString()}</option>`;
        });
      }

      shortHour(d) {
        const amPm = d >= 12 ? 'PM' : 'AM';
        return `${(d % 12) || 12}${amPm}`;
      }

      fetchData(fileName) {
        if (!fileName) {
          return;
        }

        this.currentFile = fileName;
        this.currentDate = fileName.replace(/log-(\d{4}-\d{2}-\d{2}).csv/, '$1');

        this.picker.value = fileName;

        d3.csv(
          `${this.server}data/${fileName}`,
          (d) => {
            return {
              time: d3.timeParse('%H:%M:%S')(`${this.currentDate} ${d.time}`),
              hour: parseInt(d.time.split(':')[0]),
              intensity: d.intensity,
              duration: d.duration
            }
          },
          (data) => {
            // ignore impossible outliers
            data = data.filter(item => {
              return item.intensity < 3;
            });

            this.currentData = data;

            this.drawGraph();
          }
        );
      }

      drawGraph() {
        var totalWidth = this.graph.offsetWidth;
        var totalHeight = this.graph.offsetHeight;

        var margin = { top: 20, right: 10, bottom: 40, left: 70 },
          width = totalWidth - margin.left - margin.right,
          height = totalHeight - margin.top - margin.bottom;

        this.graph.innerHTML = '';
        var svg = d3.select('#browser-viewer')
          .append('svg')
          .attr('viewBox', `0 0 ${totalWidth} ${totalHeight}`)
          .append('g')
          .attr('transform',
            'translate(' + margin.left + ',' + margin.top + ')');

        const topTen = this.currentData.slice().sort((a, b) => {
          return b.intensity - a.intensity;
        }).slice(0, 10);

        this.hallOfFame.innerHTML = '';
        for (let item of topTen) {
          this.hallOfFame.innerHTML += `
                <tr>
                  <td>${this.shortHour(item.hour)}</td>
                  <td>${parseFloat(item.intensity).toFixed(2)}</td>
                  <td>${item.duration}ms</td>
                </tr>
              `;
        }

        const hours = {};
        this.currentData.forEach(item => {
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

        const data = Object.values(hours);
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
          .call(d3.axisBottom(x).tickSizeOuter(0).tickFormat(this.shortHour));

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
              const text = tick.querySelector('text');
              text.style.display = 'none';
            }
            else {
              lastRect = currentRect;
            }
          }
        }
      }
    }

    let client;
    let ui;
    let browser;
    window.addEventListener('load', () => {
      ui = new UI();
      client = new Client(window.server);
      client.addEventListener('data', (event) => {
        if (event.detail.type === 'u') {
          ui.storeData(event.detail.data);
        } else if (event.detail.type === 'i') {
          const date = new Date(event.detail.data[0] * 1000);
          const duration = parseInt(event.detail.data[1], 10);
          const intensity = parseFloat(event.detail.data[2]);
          // console.log(`Stomp ${date}: ${intensity} for ${duration}`);
          ui.updateText(intensity);
        }
      });

      client.addEventListener('open', (event) => {
        ui.start();
      });

      client.addEventListener('close', (event) => {
        ui.stop();
      });

      window.addEventListener('calibrate', () => {
        console.log('Requesting calibration');
        client.sendMessage('calibrate');
      });

      browser = new Browser(window.server);
    });
  </script>
</body>

</html>
)rawliteral";