# Project Summary: CO2 Turtle

## Overview
A C++ firmware for an ESP32 FireBeetle32 that measures indoor air quality
(CO2, temperature, humidity, IAQ) and displays the data on an E-Ink display
and a WS2812B LED strip. Configuration is done via a web interface.
The project is built with PlatformIO (Arduino framework).

---

## Hardware

| Component | Details |
|---|---|
| MCU | DFRobot FireBeetle32 (ESP32 WROOM-32D) |
| BME680 | Temperature, humidity, pressure, IAQ via I2C (SDA: GPIO21, SCL: GPIO22) |
| MH-Z19B | CO2 sensor via HardwareSerial2 (RX: GPIO17, TX: GPIO16) |
| E-Ink display | GxEPD2 213 Z98c (122x250px, 3-color) via SPI (DC: GPIO27, CS: GPIO5, CLK: GPIO18, DIN: GPIO23) |
| LED strip | WS2812B, 38 LEDs on GPIO4, powered by 5V |
| Power | 5V external PSU; ESP and sensors (except MH-Z19B) run on 3.3V from ESP onboard regulator |

**BME680 wiring note:** CS pin is hardwired to 3.3V (forces I2C mode).
SDO is floating → fixed I2C address 0x77.

---

## Software Stack

| Library | Purpose |
|---|---|
| BSEC v1.8 (Bosch) | BME680 IAQ processing, runs at LP mode (3s sample rate) |
| GxEPD2 | E-Ink display driver |
| ESPAsyncWebServer | Async web interface for configuration |
| AsyncMqttClient | Optional MQTT publishing (Home Assistant discovery supported) |
| FastLED | WS2812B LED strip control |
| MH-Z19 (wifwaf) | CO2 sensor library |
| LittleFS | Filesystem for web interface static files |
| Preferences (ESP32) | NVS storage for runtime configuration |
| EEPROM (ESP32) | BSEC IAQ calibration state persistence |

---

## Project Structure

```
src/
├── main.cpp                – Setup and main loop
├── Configuration.h         – Compile-time pin definitions and defaults
├── Credentials.h           – WiFi/MQTT credentials (not in repo, see Credentials_example.h)
├── BME680Handler.cpp/.h    – BSEC sensor reading, EEPROM state persistence
├── MHZ19Handler.cpp/.h     – CO2 sensor reading with error recovery
├── EPDHandler.cpp/.h       – E-Ink display rendering (vertical/horizontal layout)
├── LEDHandler.cpp/.h       – LED strip status display
├── LEDsection.h            – LED section definitions (enum + start/end indices)
├── WiFiHandler.cpp/.h      – WiFi init, reconnect, AP fallback mode
├── WebServerHandler.cpp/.h – Async web server, all HTTP handlers
├── MqttClientHandler.cpp/.h– MQTT publishing and Home Assistant discovery
├── ConfigHandler.cpp/.h    – Runtime config via NVS (Preferences)
└── DataCO2.cpp/.h          – Value object for MH-Z19B readings
data/static/                – Web interface HTML/CSS files (served from LittleFS)
```

---

## Architecture

All handlers are **Singletons** with `getInstance()`. Each handler manages
its own update interval internally via `updateSensorData(currentSeconds)` or
similar methods. The main loop calls all handlers every cycle and passes
`millis()/1000` as `currentSeconds`.

Data flows like this:
```
BME680Handler → Bsec object    ─┐
MHZ19Handler  → DataCO2 object ─┼→ EPDHandler, LEDHandler, WebServerHandler, MqttClientHandler
```

---

## Configuration System

Runtime configuration is stored in NVS via the `ConfigHandler` singleton.
On first boot, defaults from `Configuration.h` are written to NVS.
The web interface allows changing all settings at runtime without recompiling.

Config is split into 5 maps:

| Map | Keys |
|---|---|
| Switch | switchWIFI, switchEPD, switchEPDorientation, switchLED, switchMQTT |
| Interval (seconds) | intervalMHZ19, intervalBME680, intervalWiFi, intervalEPD, intervalLED, intervalMQTT, intervalPRINT |
| Device | deviceName, timezone, wlanSSID, wlanPASSWORD, mqttHOST, mqttPORT, mqttUSER, mqttPASSWORD, mqttUSERen |
| LED | LEDbrightness (range 2–255) |
| Sensor | pressure (hPa), tempOffset (stored as int × 10) |

---

## LED Sections (WS2812B, 38 LEDs)

| Section | LEDs | Indicates |
|---|---|---|
| LED_HUM | 0–16 | Humidity (color gradient: DarkRed → Green → Blue → White) |
| LED_WLANCONNECT | 18 | WiFi: LightSkyBlue = connected, blinking DarkRed = disconnected |
| LED_SENSORSTATE | 19 | BME680: Green = OK, Orange = recovering, Red = failed |
| LED_CO2 | 21–37 | CO2 ppm (Blue → Green → Yellow → Orange → Red → blinking Magenta) |

---

## BME680 Handler Details

- BSEC config: `generic_33v_3s_4d` (3.3V, 3s sample rate, 4 days burn-in)
- BSEC IAQ calibration state is persisted to EEPROM:
  - First save: when IAQ accuracy reaches 3
  - Periodic saves: every 360 minutes
  - On startup: state is restored from EEPROM if valid
- Onboard LED (LED_BUILTIN) lights up briefly during EEPROM state writes
- Runtime recovery: if 5 consecutive errors occur, the handler resets
  the I2C bus and reinitializes the sensor, retrying every 60 seconds

---

## MH-Z19B Handler Details

- Uses HardwareSerial2 at 9600 baud
- Auto-calibration enabled
- Error recovery: after 3 consecutive errors, Serial2 is restarted
- `DataCO2` stores: regular CO2, raw CO2, limited CO2, background CO2,
  temp adjustment, temperature, accuracy

---

## E-Ink Display

- Supports two layouts selectable via `switchEPDorientation`:
  vertical (90°/270°) and horizontal (90°/270°)
- Shows: CO2 (ppm), temperature (°C), humidity (%), IAQ, date, time, device name
- Color thresholds are used for alert coloring (red/black highlights)
- Forced full refresh at configurable interval (`intervalEPD`, default 900s)

---

## MQTT (optional, disabled by default)

- Home Assistant MQTT discovery support via `publishDiscovery()`
- Publishes all sensor values at `intervalMQTT`
- Optional user/password authentication
- Enable via `switchMQTT` in web interface or `Configuration.h`

---

## Important Notes

- `Credentials.h` is not in the repository. Copy `Credentials_example.h`
  and fill in WiFi and MQTT credentials before building.
- If WiFi credentials are missing, the device falls back to AP mode.
- `DEBUG` mode (set in `Configuration.h`) enables verbose Serial output
  and RAM usage reporting.
- The MH-Z19B is powered by 5V directly from the PSU, not from the ESP.

---

## How I Want Code Changes Delivered

Please structure every code change so that it fits into a single Git commit
and can be applied manually. This means:

1. **The exact code change** – show clearly what to remove (ALT) and what
   to replace it with (NEU), including enough surrounding context to locate
   the change unambiguously in the file. Always include the filename.

2. **A commit message** in the format:
   ```
   type(scope): short description

   Longer explanation if needed.
   ```
   Common types: `feat`, `fix`, `refactor`, `docs`, `chore`

If a larger change logically consists of multiple independent commits,
split it into separate blocks – one (ALT/NEU + commit message) per commit.
