# ESP 32 Weather Station
This weather station provides a dashboard for measuring humidity, temperature, and heat index using the DHT11 temperature sensor
## Features
- Has a web dashboard that can be connected through a WiFi router.
- Calculates heat index automatically.
## Hardware Required
- ESP32 Development Board (e.g., ESP32-WROOM-32)
- DHT11 Temperature Sensor
- Jumper wires & breadboard
## Wiring / Pinout
Connect your components to the ESP32 using the following pins:

| Component Pin | ESP32 Pin | Notes |
| :--- | :--- | :--- |
| VCC | 3.3V | Do not use 5V if sensor is 3.3V only |
| GND | GND | |
| Data / SDA | GPIO 4 | |

## Intallation/Usage:
1. Clone this repo (Git is required):
   
   ```bash
   git clone https://github.com/LeouigiKyle-31/ESP32-Weather-Station.git
   ```
2. Open the folder using vscode with PlatformIO.
3. Select your ESP32 board and COM port, then click **Upload**.
4. Open the **Serial Monitor** at `115200` baud to see the output.



