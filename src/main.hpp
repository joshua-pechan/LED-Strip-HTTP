#pragma once

#include <FastLED.h>
#include <string>
#include <map>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include <WiFiManager.hpp>

// wifi
#define SSID "2812Agilny"
#define PASSWORD "hailey123"

#define DEBUG_LED 4

// leds
#define DATA_PIN 5
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS 150
extern CRGB leds[];

#define BRIGHTNESS 96
#define FRAMES_PER_SECOND 120

extern std::string currentPattern;
extern uint8_t hue;

void wifiConnect();

void rainbow();
void rainbowWithGlitter();
void addGlitter(fract8 chanceOfGlitter);
void confetti();
void sinelon();
void bpm();
void juggle();
