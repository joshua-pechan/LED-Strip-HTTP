#include "main.hpp"

ESP8266WiFiMulti wifiMulti;
EventDispatcher dispatcher;
Timer timer;
ESP8266WebServer server(80);

WiFiManager wifiManager(&wifiMulti, &dispatcher, &timer, SSID, PASSWORD);

// leds
CRGB leds[NUM_LEDS];
std::string currentPattern = "rainbow";
uint8_t hue = 0;
uint8_t brightness = BRIGHTNESS;
boolean increasingHue = false;

std::map<std::string, std::function<void()>> functionMap = {
    {"rainbow", rainbow},
    {"rainbowWithGlitter", rainbowWithGlitter},
    {"confetti", confetti},
    {"sinelon", sinelon},
    {"bpm", bpm},
    {"juggle", juggle},
    {"solid", solid}
};

void setup() {
  pinMode(DEBUG_LED, OUTPUT);
  Serial.begin(115200);
  delay(3000);

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed!");
    return;
  }

  server.serveStatic("/", LittleFS, "/");

  server.on("/state", HTTPMethod::HTTP_GET, []() {
    JsonDocument jsonDoc;
    jsonDoc["pattern"] = currentPattern.c_str();
    jsonDoc["hue"] = hue;
    jsonDoc["increasingHue"] = increasingHue;
    jsonDoc["brightness"] = brightness;
    String response;
    serializeJson(jsonDoc, response);
    server.send(200, "application/json", response);
    server.client().stop();
  });

  server.on("/", HTTPMethod::HTTP_POST, []() {
    if (!server.hasArg("plain")) {
      server.send(400, "application/json", "{\"error\": \"No body received\"}");
      server.client().stop();
      return;
    }

    String body = server.arg("plain");
    JsonDocument jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, body);

    if (error) {
      server.send(400, "application/json", "{\"error\": \"Invalid Json\"}");
      server.client().stop();
      return;
    }

    const int inputHue = jsonDoc["hue"] | -1;
    if (inputHue >= 0 && inputHue <= 255) { hue = (uint8_t)inputHue; }

    increasingHue = jsonDoc["increasingHue"] | false;

    const int inputBrightness = jsonDoc["brightness"] | -1;
    if (inputBrightness >= 0 && inputBrightness <= 255) {
      brightness = (uint8_t)inputBrightness;
      FastLED.setBrightness(brightness);
    }

    const char* inputPattern = jsonDoc["pattern"] | "";
    if (strcmp(inputPattern, "") != 0) {
      if (functionMap.find(std::string(inputPattern)) == functionMap.end()) {
        server.send(400, "application/json", "{\"error\": \"Invalid pattern\"}");
        server.client().stop();
        return;
      }
      currentPattern = std::string(inputPattern);
    }

    server.send(200, "text/plain", "Request processed successfully.");
    server.client().stop();
  });

  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
    server.client().stop();
  });

  wifiConnect();

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(brightness);
}

void loop() {
  timer.tick();

  if (functionMap.find(currentPattern) != functionMap.end()) {
    functionMap[currentPattern]();
  }

  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  if (increasingHue) { EVERY_N_MILLISECONDS(20) { hue++; } }
}

void wifiConnect() {
  wifiManager.connect([](wl_status_t status) {
    if (status != WL_CONNECTED) {
      digitalWrite(DEBUG_LED, HIGH);
      wifiManager.disconnect();
      delay(500);
      digitalWrite(DEBUG_LED, LOW);
      wifiConnect();
      return;
    }
    digitalWrite(DEBUG_LED, LOW);

    Serial.print("\nIP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp8266")) {
      Serial.println("MDNS responder started");
    }

    server.begin();

    timer.setOnLoop([]() {
      server.handleClient();
      MDNS.update();
    });

    Serial.println("HTTP server started");
  });
}
