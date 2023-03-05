# M5Stack Core 2 Stompinator

This little bad boy tracks your upstairs neighbor's stomping, shows it on the screen, logs it to the SD card, and publishes a web interface to show you when the action happens.

The interface shows a realtime data feed of the stomping, as well as high scores and a graph showing when what kind of stomping happens most. Yes, the whole interface shakes when a stomp happens, because why not?

https://user-images.githubusercontent.com/201344/222940918-864635ba-b659-4ff3-b109-4ea934ce879c.mov

It looks nice on mobile too:

<img src="https://user-images.githubusercontent.com/201344/222941117-2417e499-c9f7-4953-b16b-ecf0d763e5fa.jpeg" alt="Stompinator on moible" width="400">

## Usage

You'll need a [M5Stack Core2](https://shop.m5stack.com/products/m5stack-core2-esp32-iot-development-kit) to run this.

This project uses PlatformIO. You should probably install the [PlatformIO VSCode plugin](https://platformio.org/install/ide?install=vscode) to get started.

Create a file called `src/WiFiSecret.h` and define the following:

```c
#define SSID "mySSID"
#define PASSPHRASE "mySuperSecretPassphrase"
```

Change `platformio.ini` to have your device's `upload_port` and `monitor_port`, upload the sketch, stick the device on your ceiling, and cry because having a cute retro-futuristic interface showing how hard your neighbor stomps changes nothing.
