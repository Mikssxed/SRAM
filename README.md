# 🚽 SRAM – Smart Restroom Availability Monitor

Real-time restroom occupancy detection system using ultrasonic sensors, ESP32 microcontrollers, and a web dashboard. Built during a 3-day hackathon.

## How It Works

An ultrasonic sensor (HC-SR04) is mounted on the wall opposite the toilet. It measures the distance every second — if someone is sitting or standing, the distance drops below a calibrated threshold, and the system reports the restroom as **occupied**. The ESP32 sends this status over WiFi to a backend server, which pushes updates to a web frontend via WebSocket in real-time.

```
[HC-SR04] → [Logic Level Converter] → [ESP32] → WiFi → [Backend] → WebSocket → [Frontend]
```

## Hardware

| Component             | Model                             | Purpose                                      |
| --------------------- | --------------------------------- | -------------------------------------------- |
| Microcontroller       | ESP32 DevKit CP2102 (USB-C)       | Reads sensor, sends data over WiFi           |
| Ultrasonic Sensor     | HC-SR04                           | Measures distance to detect occupancy        |
| Logic Level Converter | BTE-927 (3.3V ↔ 5V)               | Translates voltage between ESP32 and HC-SR04 |
| Power Supply          | Xiaomi Powerbank 10000mAh (USB-C) | Powers the ESP32 (~5 days per charge)        |
| Enclosure             | Junction box 110×75×59mm          | Houses electronics on the wall               |
| Wiring                | Dupont female-female cables       | Connects all components (no soldering!)      |

## Wiring Diagram

```
[Powerbank USB-C] → [ESP32 USB-C port]

ESP32 pin 3V3   → Converter pin VCCA
ESP32 pin GND   → Converter pin GND (side A)
ESP32 pin 5V    → Converter pin VCCB
ESP32 pin GND   → Converter pin GND (side B)
ESP32 pin GPIO4 → Converter pin A0
ESP32 pin GPIO5 → Converter pin A1

Converter pin B0 → HC-SR04 pin TRIG
Converter pin B1 → HC-SR04 pin ECHO

ESP32 pin 5V    → HC-SR04 pin VCC
ESP32 pin GND   → HC-SR04 pin GND
```

## Firmware (ESP32 – Arduino C++)

## Cost

Total hardware cost: ~229 PLN (~$58 USD) for one restroom sensor.

## Team

Built with ❤️ during a 3-day hackathon.
