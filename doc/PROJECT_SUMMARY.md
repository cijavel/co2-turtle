# Project Summary: CO2 Turtle

## Overview
C++ firmware for a DFRobot FireBeetle32 (ESP32) that measures indoor air quality
(CO2, temperature, humidity, IAQ) and displays the data on a 3-color E-Ink display
and a WS2812B LED strip. All settings are configurable at runtime via a web interface
without recompiling. Built with PlatformIO (Arduino framework).

---

## Hardware

| Component | Details |
|---|---|
| MCU | DFRobot FireBeetle32 (ESP32 WROOM-32D) |
| BME680 | Temperature, humidity, pressure, IAQ via I2C (SDA: GPIO21, SCL: GPIO22) |
| MH-Z19B | CO2 sensor via HardwareSerial2 (RX: GPIO17, TX: GPIO16) |
| E-Ink display | GxEPD2 213 Z98c (122×250px, 3-color B/W/R) via SPI (DC: GPIO27, CS: GPIO5, CLK: GPIO18, DIN: GPIO23, RST: GPIO26, BUSY: GPIO25) |
| LED strip | WS2812B, 38 LEDs on GPIO4, powered by 5V |
| Power | 5V external PSU; ESP and sensors (except MH-Z19B) run on 3.3V from onboard regulator |

**BME680 wiring note:** CS pin hardwired to 3.3V (forces I2C mode). SDO floating → fixed I2C address 0x77.

**E-Ink pin note:** RST and BUSY pins are wired. `EPD_PIN_RST` and `EPD_PIN_BUSY` in `Configuration.h`
can be set to `-1` as a compile-time fallback if the pins are not connected – in that case rendering
falls back to FreeRTOS tasks to avoid blocking the async TCP stack.

---

## Software Stack

| Library | Purpose |
|---|---|
| BSEC v1.8 (Bosch) | BME680 IAQ processing, LP mode (3s sample rate) |
| GxEPD2 | E-Ink display driver |
| ESPAsyncWebServer | Async web interface |
| AsyncMqttClient | MQTT publishing (optional, Home Assistant discovery) |
| FastLED | WS2812B LED strip control |
| MH-Z19 (wifwaf) | CO2 sensor library |
| LittleFS | Filesystem for web interface static files |
| Preferences (ESP32) | NVS storage for runtime configuration |
| EEPROM (ESP32) | BSEC IAQ calibration state persistence |

---

## Project Structure

```
src/
├── main.cpp                  – Setup and main loop
├── Configuration.h           – Compile-time pin definitions, defaults, EPD pin config
├── Credentials.h             – WiFi/MQTT credentials (not in repo, see Credentials_example.h)
├── BME680Handler.cpp/.h      – BSEC sensor reading, EEPROM state persistence
├── MHZ19Handler.cpp/.h       – CO2 sensor reading with error recovery
├── EPDHandler.cpp/.h         – E-Ink rendering, 4 orientations, FreeRTOS task fallback
├── LEDHandler.cpp/.h         – LED strip status display
├── LEDsection.h              – LED section definitions (enum + start/end indices)
├── WiFiHandler.cpp/.h        – WiFi init, reconnect, AP fallback mode
├── WebServerHandler.cpp/.h   – Async web server, all HTTP handlers
├── MqttClientHandler.cpp/.h  – MQTT publishing and Home Assistant discovery
├── ConfigHandler.cpp/.h      – Runtime config via NVS (Preferences)
├── DataCO2.cpp/.h            – Value object for MH-Z19B readings
├── GxEPD2_display_selection_new_style.h – Display class/driver selection, pin wiring
└── symbol.h                  – 18×18px and 24×24px bitmap icons (PROGMEM)
data/static/                  – Web interface HTML/CSS (served from LittleFS)
  ├── index.htm / template.htm
  ├── settings.htm            – Module switches, intervals, sensor config
  ├── mqtt.htm                – MQTT connection settings
  ├── wlan.htm                – WiFi credentials
  └── style.css
```

---

## Architecture

All handlers are **Singletons** accessed via `getInstance()`. Each handler manages
its own update interval internally. The main loop calls all handlers every cycle,
passing `millis()/1000` as `currentSeconds`.

```
BME680Handler → Bsec object    ─┐
MHZ19Handler  → DataCO2 object ─┼→ EPDHandler · LEDHandler · WebServerHandler · MqttClientHandler
                                 └→ ConfigHandler (shared by all handlers)
```

---

## Configuration System

Runtime configuration is stored in NVS via `ConfigHandler`. On first boot, defaults
from `Configuration.h` are written to NVS. All settings are changeable via the web
interface without recompiling.

Config is split into 5 maps:

| Map | Keys |
|---|---|
| Switch | switchWIFI, switchEPD, switchEPDorientation (Int 0–3), switchLED, switchMQTT |
| Interval (seconds) | intervalMHZ19, intervalBME680, intervalWiFi, intervalEPD, intervalLED, intervalMQTT, intervalPRINT |
| Device | deviceName, timezone, wlanSSID, wlanPASSWORD, mqttHOST, mqttPORT, mqttUSER, mqttPASSWORD, mqttUSERen |
| LED | LEDbrightness (range 2–255) |
| Sensor | pressure (hPa), tempOffset (stored as int × 10) |

---

## E-Ink Display

The display supports 4 orientations selectable via `switchEPDorientation`:

| Value | Orientation | GxEPD2 rotation |
|---|---|---|
| 0 | Vertical 90° (cable up) | 2 |
| 1 | Vertical 270° (cable down) | 0 |
| 2 | Horizontal 90° (cable left) | 1 |
| 3 | Horizontal 270° (cable right) | 3 |

**Layout:** Two sensor columns (Temp/Hum left, CO2/IAQ right) with a footer showing
WiFi icon + SSID, IP address, date/time, and device name with turtle icon.
Alert coloring (red/black) applies per sensor value against configurable thresholds.

**Rendering:** With BUSY pin wired (`EPD_PIN_BUSY >= 0`), `printLayout()` and
`wipeDisplay()` are called directly – GxEPD2 uses the real BUSY signal (~500ms wait).
Without BUSY pin, both operations run in dedicated one-shot FreeRTOS tasks (4096 byte
stack each) to prevent blocking the async TCP stack during the ~2–3s busy-wait delay.

**Standby:** When the EPD is disabled, a sleeping turtle is shown
before the display hibernates.

**WiFi fallback:** If no WiFi connection is available, the footer shows
`"no WLAN"` / `"---"` / `"--:--"` instead of empty or `0.0.0.0` values.

---

## LED Sections (WS2812B, 38 LEDs)

| Section | LEDs | Indicates |
|---|---|---|
| LED_HUM | 0–16 | Humidity (color gradient: DarkRed → Green → Blue → White) |
| LED_WLANCONNECT | 18 | WiFi: LightSkyBlue = connected, blinking DarkRed = disconnected |
| LED_SENSORSTATE | 19 | BME680: Green = OK, Orange = recovering, Red = failed |
| LED_CO2 | 21–37 | CO2 ppm (Blue → Green → Yellow → Orange → Red → blinking Magenta) |

---

## BME680 Handler

- BSEC config: `generic_33v_3s_4d` (3.3V, 3s sample rate, 4 days burn-in)
- IAQ calibration persisted to EEPROM:
  - First save: when IAQ accuracy reaches 3
  - Periodic saves: every 360 minutes
  - On startup: state restored from EEPROM if valid
- Onboard LED (LED_BUILTIN) lights briefly during EEPROM writes
- Recovery: after 5 consecutive errors, I2C bus reset + sensor reinitialization, retrying every 60s

---

## MH-Z19B Handler

- HardwareSerial2 at 9600 baud
- Auto-calibration enabled
- Recovery: after 3 consecutive errors, Serial2 is restarted
- `DataCO2` stores: regular CO2, raw CO2, limited CO2, background CO2, temp adjustment, temperature, accuracy

---

## MQTT (optional, disabled by default)

- Home Assistant MQTT discovery via `publishDiscovery()` on connect
- Publishes all sensor values at `intervalMQTT`
- Optional user/password authentication (`mqttUSERen`)
- Host, port, credentials fully configurable via web UI without recompiling

---

## Important Notes

- `Credentials.h` is **not in the repository**. Copy `Credentials_example.h` and fill in
  WiFi/MQTT credentials before building.
- If WiFi credentials are missing or connection fails, the device falls back to AP mode.
- `DEBUG 1` in `Configuration.h` enables verbose Serial output and RAM usage reporting.
- The MH-Z19B is powered by 5V directly from the PSU, not from the ESP.
- `switchEPDorientation` is stored as **Int** (not Bool) to support values 0–3.

---

## How I Want Code Changes Delivered

Structure every code change so it fits into a single Git commit and can be applied manually:

1. **The exact code change** – show what to remove (ALT) and replace it with (NEU),
   including enough surrounding context to locate the change unambiguously. Always include the filename.

2. **A commit message** in the format:
   ```
   type(scope): short description

   Longer explanation if needed.
   ```
   Common types: `feat`, `fix`, `refactor`, `docs`, `chore`

If a larger change consists of multiple independent commits, split into separate
ALT/NEU + commit message blocks – one per commit.

- The user decides which changes to apply. Do not assume a suggested
  change was implemented unless the user explicitly confirms it.
