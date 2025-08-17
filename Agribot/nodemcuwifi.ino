// NodeMCU Code (I2C Master and Wi-Fi Web Server)
// This code runs on the NodeMCU and provides a web interface to control the robot.
// It sends commands to the Arduino Uno via I2C.

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h> // Library for I2C communication

// Replace with your network credentials
const char* ssid = "George4G";
const char* password = "aceraspire5050";

// Define the I2C address of the Arduino slave
const int arduinoI2CAddress = 0x8;

// Create a web server on port 80
ESP8266WebServer server(80);

// Function declarations to solve the compilation errors
void handleRoot();
void handleMove();
void handleSower();
void handlePump();
void handleSensor();

void setup() {
  Serial.begin(9600);
  
  // Set up I2C for the NodeMCU as a master
  // D2 is GPIO4 (SDA), D1 is GPIO5 (SCL)
  Wire.begin(4, 5);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Define the web server routes
  server.on("/", handleRoot);
  server.on("/move", handleMove);
  server.on("/sower", handleSower);
  server.on("/pump", handlePump);
  server.on("/sensor", handleSensor);
  
  // Start the web server
  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  server.handleClient();
}

// Function to send a command to the Arduino via I2C
void sendCommand(char command) {
  Wire.beginTransmission(arduinoI2CAddress);
  Wire.write(command);
  Wire.endTransmission();
}

// Function to handle the root URL (the main web page)
void handleRoot() {
  // This is the complete HTML, CSS, and JavaScript for the web app.
  // It uses Tailwind CSS for styling and JavaScript to send commands via HTTP.
  String html = R"rawliteral(
    <!DOCTYPE: html>
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>Agri-Robot Control</title>
      <script src="https://cdn.tailwindcss.com"></script>
      <style>
        body { font-family: 'Inter', sans-serif; }
        .grid-container { display: grid; grid-template-areas: ". forward ." "left stop right" ". backward ."; }
        .forward { grid-area: forward; }
        .backward { grid-area: backward; }
        .left { grid-area: left; }
        .right { grid-area: right; }
        .stop { grid-area: stop; }
      </style>
    </head>
    <body class="bg-gray-100 text-gray-800 p-4 min-h-screen flex flex-col items-center justify-center">
      <div class="max-w-xl w-full bg-white rounded-xl shadow-lg p-6">
        <h1 class="text-3xl font-bold text-center mb-6 text-gray-900">Agri-Robot Controls</h1>

        <!-- Movement Controls -->
        <div class="mb-8">
          <h2 class="text-xl font-semibold mb-4 text-center">Movement</h2>
          <div class="grid-container w-48 h-48 mx-auto gap-2">
            <button class="forward p-3 rounded-lg bg-indigo-500 hover:bg-indigo-600 text-white font-bold" onclick="sendCommand('F')">Forward</button>
            <button class="left p-3 rounded-lg bg-indigo-500 hover:bg-indigo-600 text-white font-bold" onclick="sendCommand('L')">Left</button>
            <button class="stop p-3 rounded-lg bg-red-500 hover:bg-red-600 text-white font-bold" onclick="sendCommand('S')">Stop</button>
            <button class="right p-3 rounded-lg bg-indigo-500 hover:bg-indigo-600 text-white font-bold" onclick="sendCommand('R')">Right</button>
            <button class="backward p-3 rounded-lg bg-indigo-500 hover:bg-indigo-600 text-white font-bold" onclick="sendCommand('B')">Backward</button>
          </div>
        </div>

        <!-- Module Controls -->
        <div class="mb-8">
          <h2 class="text-xl font-semibold mb-4 text-center">Modules</h2>
          <div class="grid grid-cols-2 gap-4">
            <button class="p-3 rounded-lg text-white font-bold" id="seedBtn" onclick="toggleModule('sower')">Sow Seeds (Off)</button>
            <button class="p-3 rounded-lg text-white font-bold" id="pumpBtn" onclick="toggleModule('pump')">Water Pump (Off)</button>
          </div>
        </div>

        <!-- Sensor Data -->
        <div class="text-center">
          <h2 class="text-xl font-semibold mb-2">Sensor Data</h2>
          <button id="getSensorBtn" class="p-3 rounded-lg bg-green-500 hover:bg-green-600 text-white font-bold" onclick="getSensorData()">Get Data</button>
          <div id="sensorData" class="mt-4 p-4 bg-gray-200 rounded-lg text-lg font-mono">
            Click 'Get Data' to read sensor...
          </div>
        </div>
      </div>

      <script>
        const seedBtn = document.getElementById('seedBtn');
        const pumpBtn = document.getElementById('pumpBtn');
        const getSensorBtn = document.getElementById('getSensorBtn');
        const sensorData = document.getElementById('sensorData');

        let seedState = false; // false = off, true = on
        let pumpState = false; // false = off, true = on

        seedBtn.style.backgroundColor = '#9CA3AF'; // Gray for off
        pumpBtn.style.backgroundColor = '#9CA3AF'; // Gray for off

        function sendCommand(dir) {
          fetch(`/move?dir=${dir}`);
        }

        function toggleModule(module) {
          if (module === 'sower') {
            seedState = !seedState;
            seedBtn.style.backgroundColor = seedState ? '#10B981' : '#9CA3AF';
            seedBtn.innerText = `Sow Seeds (${seedState ? 'On' : 'Off'})`;
            fetch(`/sower?state=${seedState ? 'on' : 'off'}`);
          } else if (module === 'pump') {
            pumpState = !pumpState;
            pumpBtn.style.backgroundColor = pumpState ? '#3B82F6' : '#9CA3AF';
            pumpBtn.innerText = `Water Pump (${pumpState ? 'On' : 'Off'})`;
            fetch(`/pump?state=${pumpState ? 'on' : 'off'}`);
          }
        }

        async function getSensorData() {
          sensorData.innerText = 'Loading...';
          getSensorBtn.disabled = true; // Disable button during request
          try {
            const response = await fetch('/sensor');
            const text = await response.text();
            sensorData.innerText = text;
          } catch (error) {
            sensorData.innerText = 'Error getting data.';
            console.error('Fetch error:', error);
          }
          getSensorBtn.disabled = false;
        }
      </script>
    </body>
    </html>
    )rawliteral";
  server.send(200, "text/html", html);
}

// Handle movement commands from the web app
void handleMove() {
  String dir = server.arg("dir");
  if (dir == "F") {
    sendCommand('F');
  } else if (dir == "B") {
    sendCommand('B');
  } else if (dir == "L") {
    sendCommand('L');
  } else if (dir == "R") {
    sendCommand('R');
  } else if (dir == "S") {
    sendCommand('S');
  }
  server.send(200, "text/plain", "OK");
}

// Handle seed sower commands
void handleSower() {
  String state = server.arg("state");
  if (state == "on") {
    sendCommand('D'); // D for 'Dig' or 'Sow'
  } else {
    sendCommand('C'); // C for 'Cease'
  }
  server.send(200, "text/plain", "OK");
}

// Handle water pump commands
void handlePump() {
  String state = server.arg("state");
  if (state == "on") {
    sendCommand('P');
  } else {
    sendCommand('O');
  }
  server.send(200, "text/plain", "OK");
}

// Handle sensor data requests from the web app
void handleSensor() {
  // Request data from Arduino
  Wire.requestFrom(arduinoI2CAddress, 100); // Request up to 100 bytes
  
  String sensorData = "";
  while(Wire.available()){
    char c = Wire.read();
    sensorData += c;
  }
  
  if (sensorData.length() > 0) {
    server.send(200, "text/plain", sensorData);
  } else {
    server.send(500, "text/plain", "No data received from Arduino.");
  }
}
