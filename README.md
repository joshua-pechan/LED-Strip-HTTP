# LED Strip HTTP Website
Allows for pattern and hue changes for an LED strip with a simple interface on a website hosted by an ESP8266. This is for personal use — LED strips hung in diffusion channels on the outside of my house.

## Setup

1. Copy `src/secrets.hpp.example` to `src/secrets.hpp` and fill in your WiFi credentials.
2. Flash the firmware with PlatformIO: `pio run --target upload`
3. Upload the filesystem: `pio run --target uploadfs`
4. Access the UI at `http://esp8266.local` (or the device IP printed over serial).

## Features

- **Hue slider** — sets the base color (0–255 maps to full hue wheel)
- **Brightness slider** — adjusts LED brightness at runtime (0–255)
- **Increasing Hue** — auto-cycles through colors over time
- **Patterns** — Solid, Rainbow, Rainbow With Glitter, Confetti, Sinelon, BPM, Juggle
- UI syncs to device state on page load

## Tech Stack

**Client:** LittleFS (minified HTML, JS)

**Server:** ESP8266 (C++, FastLED, ArduinoJson)

## Optimizations
JS and HTML are minified because the flash size was smaller than initially expected. Before minification, the code was being cut off at runtime if it exceeded available space.
