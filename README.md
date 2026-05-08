# SRAM – Smart Restroom Availability Monitor

Real-time restroom occupancy detection system using ultrasonic sensors, ESP32 microcontrollers, and a Vue.js web dashboard. Built during a 3-day hackathon.

## How It Works

An HC-SR04 ultrasonic sensor measures distance every few seconds. If someone is present, the distance drops below a calibrated threshold and the ESP32 reports the stall as occupied. The ESP32 sends data directly to Firebase Realtime Database over WiFi. The Vue.js frontend reads from Firebase endpoints to display live occupancy status.

```
[HC-SR04] → [ESP32] → WiFi → [Firebase Realtime DB] → [Vue.js Frontend]
```

## Stack

- **Firmware** — Arduino C++ on ESP32, deep sleep between readings for battery efficiency
- **Backend** — Firebase Realtime Database (no custom server)
- **Frontend** — Vue 3 + Vite, reads Firebase REST endpoints, displays stall status with live clock, busy hours chart, and battery/night-mode indicators

## Hardware

| Component             | Model                             | Purpose                                      |
| --------------------- | --------------------------------- | -------------------------------------------- |
| Microcontroller       | ESP32 DevKit CP2102 (USB-C)       | Reads sensor, sends data over WiFi           |
| Ultrasonic Sensor     | HC-SR04                           | Measures distance to detect occupancy        |
| Logic Level Converter | BTE-927 (3.3V ↔ 5V)               | Translates voltage between ESP32 and HC-SR04 |
| Power Supply          | Xiaomi Powerbank 10000mAh (USB-C) | Powers the ESP32 (~5 days per charge)        |
| Enclosure             | Junction box 110×75×59mm          | Houses electronics on the wall               |
| Wiring                | Dupont female-female cables       | Connects all components (no soldering!)      |

## Wiring

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

## Team

[https://github.com/msern]
[https://github.com/mmoszczenski]
[https://github.com/dofranko]
[https://github.com/Mikssxed]

Built with ❤️ during a 3-day hackathon.
