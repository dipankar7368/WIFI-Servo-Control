#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

const char* AP_SSID = "ESP32-Spray";
const char* AP_PASSWORD = "12345678";

#define SERVO_PIN 3

Servo sprayServo;
WebServer server(80);

// Servo positions
int REST_ANGLE = 20;
int PRESS_ANGLE = 100;

// Timing
int PRESS_TIME = 500;
int RELEASE_TIME = 500;

bool spraying = false;

// ---------------- HTML PAGE ----------------

const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">

<title>ESP32 Spray Controller</title>

<style>
body {
  margin: 0;
  font-family: Arial, sans-serif;
  background: #101820;
  color: white;
  text-align: center;
}

.container {
  max-width: 450px;
  margin: auto;
  padding: 20px;
}

h1 {
  margin-bottom: 5px;
}

.status {
  background: #1d2b36;
  padding: 12px;
  border-radius: 12px;
  margin: 20px 0;
}

button {
  width: 100%;
  padding: 18px;
  margin: 8px 0;
  border: none;
  border-radius: 14px;
  font-size: 20px;
  font-weight: bold;
  cursor: pointer;
}

.spray {
  background: #00c853;
  color: white;
}

.stop {
  background: #ff1744;
  color: white;
}

.small {
  background: #2979ff;
  color: white;
}

.settings {
  background: #1d2b36;
  padding: 15px;
  border-radius: 15px;
  margin-top: 20px;
}

input[type=range] {
  width: 100%;
}

.value {
  font-size: 18px;
  color: #00e5ff;
}
</style>
</head>

<body>

<div class="container">

<h1>💧 Wi-Fi Spray</h1>
<p>ESP32-C3 + SG90</p>

<div class="status">
  Status: <b id="status">Ready</b>
</div>

<button class="spray" onclick="spray(1)">
💧 1 SPRAY
</button>

<button class="spray" onclick="spray(2)">
💧💧 2 SPRAYS
</button>

<button class="spray" onclick="spray(3)">
💧💧💧 3 SPRAYS
</button>

<button class="stop" onclick="stopSpray()">
🛑 STOP
</button>

<div class="settings">

<h2>⚙️ Settings</h2>

<p>
Press Angle:
<br>
<span class="value" id="angleValue">100°</span>
</p>

<input
type="range"
min="50"
max="150"
value="100"
oninput="updateAngle(this.value)"
>

<p>
Press Time:
<br>
<span class="value" id="timeValue">500 ms</span>
</p>

<input
type="range"
min="100"
max="2000"
step="100"
value="500"
oninput="updateTime(this.value)"
>

</div>

</div>

<script>

function spray(count) {

  document.getElementById("status").innerHTML =
  "Spraying...";

  fetch("/spray?count=" + count)
  .then(response => response.text())
  .then(data => {

    document.getElementById("status").innerHTML =
    data;

  });
}

function stopSpray() {

  fetch("/stop")
  .then(response => response.text())
  .then(data => {

    document.getElementById("status").innerHTML =
    data;

  });
}

function updateAngle(value) {

  document.getElementById("angleValue").innerHTML =
  value + "°";

  fetch("/angle?value=" + value);
}

function updateTime(value) {

  document.getElementById("timeValue").innerHTML =
  value + " ms";

  fetch("/time?value=" + value);
}

</script>

</body>
</html>
)rawliteral";

// ---------------- SPRAY FUNCTION ----------------

void doSpray(int count) {

  spraying = true;

  for (int i = 0; i < count; i++) {

    if (!spraying) {
      break;
    }

    // Press pump
    sprayServo.write(PRESS_ANGLE);
    delay(PRESS_TIME);

    if (!spraying) {
      break;
    }

    // Release pump
    sprayServo.write(REST_ANGLE);
    delay(RELEASE_TIME);
  }

  sprayServo.write(REST_ANGLE);
  spraying = false;
}

// ---------------- WEB HANDLERS ----------------

void handleRoot() {

  server.send(
    200,
    "text/html",
    webpage
  );
}

void handleSpray() {

  if (!server.hasArg("count")) {
    server.send(400, "text/plain", "Invalid count");
    return;
  }

  int count = server.arg("count").toInt();

  if (count < 1) count = 1;
  if (count > 10) count = 10;

  doSpray(count);

  server.send(
    200,
    "text/plain",
    "Spray Complete ✓"
  );
}

void handleStop() {

  spraying = false;

  sprayServo.write(REST_ANGLE);

  server.send(
    200,
    "text/plain",
    "Stopped"
  );
}

void handleAngle() {

  if (server.hasArg("value")) {

    int value = server.arg("value").toInt();

    value = constrain(value, 0, 180);

    PRESS_ANGLE = value;

    server.send(
      200,
      "text/plain",
      "Angle Updated"
    );

  } else {

    server.send(
      400,
      "text/plain",
      "Invalid angle"
    );
  }
}

void handleTime() {

  if (server.hasArg("value")) {

    int value = server.arg("value").toInt();

    value = constrain(value, 100, 2000);

    PRESS_TIME = value;

    server.send(
      200,
      "text/plain",
      "Time Updated"
    );

  } else {

    server.send(
      400,
      "text/plain",
      "Invalid time"
    );
  }
}

// ---------------- SETUP ----------------

void setup() {

  Serial.begin(115200);

  // Servo setup
  sprayServo.setPeriodHertz(50);

  sprayServo.attach(
    SERVO_PIN,
    500,
    2400
  );

  sprayServo.write(REST_ANGLE);

  // Start Wi-Fi Access Point
  WiFi.mode(WIFI_AP);

  WiFi.softAP(
    AP_SSID,
    AP_PASSWORD
  );

  IPAddress IP = WiFi.softAPIP();

  Serial.println();
  Serial.println("==========================");
  Serial.println("ESP32 Spray Controller");
  Serial.println("==========================");

  Serial.print("WiFi Name: ");
  Serial.println(AP_SSID);

  Serial.print("Password: ");
  Serial.println(AP_PASSWORD);

  Serial.print("Web Address: ");
  Serial.println(IP);

  // Web server
  server.on("/", handleRoot);

  server.on("/spray", handleSpray);

  server.on("/stop", handleStop);

  server.on("/angle", handleAngle);

  server.on("/time", handleTime);

  server.begin();

  Serial.println("Web server started!");
}

// ---------------- LOOP ----------------

void loop() {

  server.handleClient();
}
