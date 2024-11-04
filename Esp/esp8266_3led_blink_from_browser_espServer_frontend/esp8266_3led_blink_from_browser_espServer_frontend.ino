#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "";
const char* password = "";

ESP8266WebServer server(80);

// LED Pins
const int ledPins[] = {5, 4, 0}; // Define GPIO pins for each LED
int selectedLed = 5;
int delayTime = 500;

const char* webpage = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP8266 LED Controller</title>
</head>
<body>
  <h2>ESP8266 LED Controller</h2>
  <label>Select LED:</label>
  <select id="ledSelect">
    <option value="5">LED 1</option>
    <option value="4">LED 2</option>
    <option value="0">LED 3</option>
  </select>
  <br><br>
  <label>Delay Time (ms):</label>
  <input type="range" id="delayRange" min="100" max="2000" value="500">
  <span id="delayValue">500</span> ms
  <br><br>
  <button onclick="updateLED()">Set LED</button>
  <script>
    const delayRange = document.getElementById("delayRange");
    const delayValue = document.getElementById("delayValue");
    delayRange.oninput = () => delayValue.textContent = delayRange.value;

    function updateLED() {
      const led = document.getElementById("ledSelect").value;
      const delay = delayRange.value;
      fetch(`/update?led=${led}&delay=${delay}`)
        .then(response => response.text())
        .then(data => console.log(data));
    }
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Print the IP address
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());  // Print the assigned IP address
  
  for (int i = 0; i < 3; i++) {
    pinMode(ledPins[i], OUTPUT);
  }

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", webpage);
  });

  server.on("/update", HTTP_GET, []() {
    selectedLed = server.arg("led").toInt();
    delayTime = server.arg("delay").toInt();
    server.send(200, "text/plain", "Updated");
  });

  server.begin();
}

void loop() {
  server.handleClient();

  digitalWrite(selectedLed, HIGH);
  delay(delayTime);
  digitalWrite(selectedLed, LOW);
  delay(delayTime);
}
