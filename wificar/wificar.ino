#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WebSocketsServer.h>
#include <Servo.h>
#include <ArduinoJson.h>

const char* ssid = "Your WiFi name";
const char* password = "Your WiFi password";

const int ledPin = D6;
const int motorSpeedPin = D1; // PWM pin for controlling motor speed
const int motorDirectionPin = D2; // Pin for controlling motor direction
const int servoPin = D5;

bool ledState = false;
Servo servo;

ESP8266WebServer server(80);
WebSocketsServer webSocket(81);

void handleRoot() {
  String html ="<!DOCTYPE html>"
                "<html>"
                "<head>"
                "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                "<style>"
                "  canvas {"
                "    border: 1px solid black;"
                "  }"
                "</style>"
                "<script>"
                "  let socket;"
                "  function init() {"
                "    socket = new WebSocket('ws://' + location.hostname + ':81/');"
                "    socket.onopen = function(event) {"
                "      console.log('WebSocket connected');"
                "    };"
                "    var canvas = document.getElementById('controlCanvas');"
                "    canvas.width = window.innerWidth;"
                "    canvas.height = canvas.width;"
                "    draw();"
                "  }"
                "  function toggleLED() {"
                "    socket.send('toggleLED');"
                "  }"
                "let canvas, ctx, centerX, centerY, circleRadius;"
                "let thumbX, thumbY, isDragging = false;"

                ""
                "function init() {"
                "  canvas = document.getElementById('controlCanvas');"
                "  ctx = canvas.getContext('2d');"
                "  centerX = canvas.width / 2;"
                "  centerY = canvas.height / 2;"
                "  circleRadius = 50;"
                ""
                "  thumbX = centerX;"
                "  thumbY = centerY;"
                ""
                "  canvas.addEventListener('touchstart', handleTouchStart, false);"
                "  canvas.addEventListener('touchmove', handleTouchMove, false);"
                "  canvas.addEventListener('touchend', handleTouchEnd, false);"
                ""
                "  socket = new WebSocket('ws://' + window.location.hostname + ':81');"
                ""
                "  draw();"
                "}"
                ""
                "function handleTouchStart(e) {"
                "  e.preventDefault();"
                "  isDragging = true;"
                "}"
                ""
                "function handleTouchMove(e) {"
                "  if (!isDragging) return;"
                "  e.preventDefault();"
                ""
                "  let rect = canvas.getBoundingClientRect();"
                "  let touch = e.touches[0];"
                "  thumbX = touch.clientX - rect.left;"
                "  thumbY = touch.clientY - rect.top;"
                ""
                "  let dx = thumbX - centerX;"
                "  let dy = thumbY - centerY;"
                "  let distance = Math.sqrt(dx * dx + dy * dy);"
                ""
                "  if (distance > circleRadius) {"
                "    thumbX = centerX + (dx / distance) * circleRadius;"
                "    thumbY = centerY + (dy / distance) * circleRadius;"
                "  }"
                ""
                "  sendControl(dx / circleRadius, dy / circleRadius);"
                ""
                "  draw();"
                "}"
                ""
                "function handleTouchEnd(e) {"
                "  e.preventDefault();"
                "  isDragging = false;"
                "  thumbX = centerX;"
                "  thumbY = centerY;"
                ""
                "  sendControl(0, 0);"
                ""
                "  draw();"
                "}"
                ""
                "function draw() {"
                "  ctx.clearRect(0, 0, canvas.width, canvas.height);"
                ""
                "  ctx.beginPath();"
                "  ctx.arc(centerX, centerY, circleRadius, 0, 2 * Math.PI);"
                "  ctx.stroke();"
                ""
                "  ctx.beginPath();"
                "  ctx.arc(thumbX, thumbY, 20, 0, 2 * Math.PI);"
                "  ctx.fill();"
                "}"
                ""
                "function sendControl(x, y) {"
                "  if (socket.readyState === WebSocket.OPEN) {"
                "    let message = {"
                "      x: x.toFixed(2),"
                "      y: y.toFixed(2)"
                "    };"
                "    socket.send(JSON.stringify(message));"
                "  }"
                "}"
               "</script>"
                "</head>"
                "<body onload='init()'>"
                "  <h1>Wi-Fi Car Control</h1>"
                "  <button onclick='toggleLED()'>Toggle LED</button><br><br>"
                "  <canvas id='controlCanvas'></canvas>"
                "</body>"
                "</html>";

  server.send(200, "text/html", html);
}

void handleToggle() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState ? HIGH : LOW);
  server.sendHeader("Location", "/");
  server.send(303, "text/plain", "");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  if (type == WStype_TEXT) {
    String message = String((char *)payload);

    if (message == "toggleLED") {
      digitalWrite(ledPin, !digitalRead(ledPin));
      return;
    }

    StaticJsonDocument<64> doc;
    deserializeJson(doc, message);

    float x = doc["x"].as<float>();
    float y = doc["y"].as<float>();

    if(x < -1)
      x = -1;
    else if(x > 1)
      x = 1;

    int servoAngle = map(x * 100, -100, 100, 0, 180);
    int motorSpeed = map(abs(y * 100), 0, 100, 0, 255);
    bool motorDirection = y >= 0;
    if(motorDirection)
      motorSpeed = 255 - motorSpeed;

    servo.write(servoAngle);
    analogWrite(motorSpeedPin, motorSpeed);
    digitalWrite(motorDirectionPin, motorDirection);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(motorSpeedPin, OUTPUT);
  pinMode(motorDirectionPin, OUTPUT);

  servo.attach(servoPin);
  servo.write(90);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("wificar")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  server.handleClient();
  webSocket.loop();
}

