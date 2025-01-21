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
boolean increasingHue = false;
boolean notConnected = true;

std::map<std::string, std::function<void()>> functionMap = {
    {"rainbow", rainbow},
    {"rainbowWithGlitter", rainbowWithGlitter},
    {"confetti", confetti},
    {"sinelon", sinelon},
    {"bpm", bpm},
    {"juggle", juggle}
};

void setup() {
  Serial.begin(115200);
  delay(3000);

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed!");
    return;
  }

  server.serveStatic("/", LittleFS, "/");

  server.on("/", HTTPMethod::HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      
      JsonDocument jsonDoc;
      DeserializationError error = deserializeJson(jsonDoc, body);

      if (error) {
        server.send(400, "application/json", "{\"error\": \"Invalid Json\"}");
        return;
      }

      const int inputHue = jsonDoc["hue"];
      if (inputHue != -1) { hue = inputHue; }

      const boolean inputIncreasingHue = jsonDoc["increasingHue"];
      increasingHue = inputIncreasingHue;

      const char* inputPattern = jsonDoc["pattern"];
      if (strcmp(inputPattern, "") != 0) { currentPattern = std::string(inputPattern); }

      server.send(200, "text/plain", "Request processed successfully.");
    } else {
      server.send(400, "application/json", "{\"error\": \"No body received\"}");
    }
    
    server.client().stop();
  });

  server.onNotFound([]() { server.send(404, "text/plain", "This is not found"); });

  wifiManager.connect([](wl_status_t status) {
    if (status != WL_CONNECTED) {
      Serial.println("could not connect to wifi");
      return;
    }

    Serial.print("\nIP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp8266")) {
      Serial.println("MDNS responder started");
    }

    server.begin();
    LittleFS.begin();

    timer.setOnLoop([]() {
      server.handleClient();
      MDNS.update();
    });

    Serial.println("HTTP server started");
  });

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() { 
  timer.tick();
  
  if (functionMap.find(currentPattern) != functionMap.end()) {
    functionMap[currentPattern]();
  }

  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);

  // change color
  if (increasingHue) { EVERY_N_MILLISECONDS(20) { hue++; } }

  // change pattern
  // EVERY_N_SECONDS( 10 ) { nextPattern(); }
}

// #define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// void nextPattern()
// {
//   currentPatternNumber = (currentPatternNumber + 1) % ARRAY_SIZE(patterns);
// }