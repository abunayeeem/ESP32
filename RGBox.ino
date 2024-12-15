#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FastLED.h>

const char *ssid = "RGBox";
const char *password = "12345678";

#define DATA_PIN 1
#define NUM_LEDS 3

CRGB leds[NUM_LEDS];

static int currentRed = 255;
static int currentGreen = 0;
static int currentBlue = 0;
static int currentBrightness = 50;

AsyncWebServer server(80);

// Custom IP configuration
IPAddress local_IP(192, 168, 4, 1);  // Custom IP address
IPAddress gateway(192, 168, 4, 1);  // Gateway (same as custom IP for AP mode)
IPAddress subnet(255, 255, 255, 0); // Subnet mask

String generateHtml() {
  String html = "<html><head><style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; }";
  html += ".color-form { display: inline-block; margin-top: 20px; }";
  html += "input[type='text'] { width: 40px; text-align: center; }";
  html += "input[type='range'] { width: 200px; margin-top: 10px; }";
  html += "input[type='button'] { width: 80px; margin-top: 10px; padding: 5px; }";
  html += "</style></head><body>";
  html += "<h1>ESP32 RGB Control</h1>";
  html += "<div class='color-form'>";
  html += "Red: <input type='range' id='redSlider' min='0' max='255' value='" + String(currentRed) + "' oninput='updateRed(this.value)'><br>";
  html += "Green: <input type='range' id='greenSlider' min='0' max='255' value='" + String(currentGreen) + "' oninput='updateGreen(this.value)'><br>";
  html += "Blue: <input type='range' id='blueSlider' min='0' max='255' value='" + String(currentBlue) + "' oninput='updateBlue(this.value)'><br>";
  html += "Brightness: <input type='range' id='brightnessSlider' min='0' max='255' value='" + String(currentBrightness) + "' oninput='updateBrightness(this.value)'><br>";
  html += "<input type='button' value='Set Color' onclick='setColor()'><br>";
  html += "<p>Current Color: R=<span id='currentRed'>" + String(currentRed) + "</span>, G=<span id='currentGreen'>" + String(currentGreen) + "</span>, B=<span id='currentBlue'>" + String(currentBlue) + "</span></p>";
  html += "<p>Current Brightness: <span id='currentBrightness'>" + String(currentBrightness) + "</span></p>";
  html += "</div></body></html>";
  html += "<script>";
  html += "function setColor() {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', `/setcolor?r=${document.getElementById('redSlider').value}&g=${document.getElementById('greenSlider').value}&b=${document.getElementById('blueSlider').value}&brightness=${document.getElementById('brightnessSlider').value}`, true);";
  html += "  xhr.send();";
  html += "}";
  html += "function updateRed(value) {";
  html += "  document.getElementById('currentRed').textContent = value;";
  html += "  document.getElementById('redSlider').value = value;";
  html += "  setColor();";
  html += "}";
  html += "function updateGreen(value) {";
  html += "  document.getElementById('currentGreen').textContent = value;";
  html += "  document.getElementById('greenSlider').value = value;";
  html += "  setColor();";
  html += "}";
  html += "function updateBlue(value) {";
  html += "  document.getElementById('currentBlue').textContent = value;";
  html += "  document.getElementById('blueSlider').value = value;";
  html += "  setColor();";
  html += "}";
  html += "function updateBrightness(value) {";
  html += "  document.getElementById('currentBrightness').textContent = value;";
  html += "  document.getElementById('brightnessSlider').value = value;";
  html += "  setColor();";
  html += "}";
  html += "</script>";
  return html;
}

void setup() {
  Serial.begin(115200);
  
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);

  // Configure custom IP address
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("AP Config Failed!");
    return;
  }

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.println("AP IP address: " + myIP.toString());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", generateHtml());
  });

  server.on("/setcolor", HTTP_GET, [](AsyncWebServerRequest *request){
    String r = request->getParam("r")->value();
    String g = request->getParam("g")->value();
    String b = request->getParam("b")->value();
    String brightness = request->getParam("brightness")->value();
    
    String response = "Color set to R=" + r + ", G=" + g + ", B=" + b + ", Brightness=" + brightness;
    request->send(200, "text/plain", response);

    currentRed = r.toInt();
    currentGreen = g.toInt();
    currentBlue = b.toInt();
    currentBrightness = brightness.toInt();

    FastLED.setBrightness(currentBrightness);
    fill_solid(leds, NUM_LEDS, CRGB(currentRed, currentGreen, currentBlue));
    FastLED.show();
  });

  server.begin();
}

void loop() {
  // Nothing to do here for now
}
