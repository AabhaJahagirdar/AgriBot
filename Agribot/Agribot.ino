// Arduino Code (I2C Slave)
// This code runs on the Arduino Uno with the L293D Motor Shield.
// It receives commands from the NodeMCU via I2C and controls the motors and sensors.

#include <Wire.h> // Library for I2C communication
#include <AFMotor.h> // Library for the L293D motor shield

// Define I2C address for this Arduino Uno (it must be a slave)
// The NodeMCU will send commands to this address.
#define I2C_SLAVE_ADDRESS 0x8

// The L293D shield has 4 motor channels: M1, M2, M3, M4
// We will define the motors based on your connections.
AF_DCMotor leftMotors(1);  // Left side wheels connected to M1
AF_DCMotor seedSowing(2);  // Seed sowing motor connected to M2
AF_DCMotor waterPump(3);   // Water pump motor connected to M3
AF_DCMotor rightMotors(4); // Right side wheels connected to M4

// A variable to store the command received from the NodeMCU.
char command = 'S'; // 'S' for Stop by default.

// --- SENSOR VARIABLES ---
// These variables will hold the sensor readings.
float soilMoisture = 0.0;
float soilTemperature = 0.0;
float soilPH = 0.0;
float soilEC = 0.0;
float soilNitrogen = 0.0;
float soilPhosphorus = 0.0;
float soilPotassium = 0.0;

// Placeholder for soil sensor connections.
// If your sensor uses a specific library, you'll need to include it here.
// For example: #include <DFRobot_PH.h>
// #include <DFRobot_EC.h>
// You would also instantiate your sensor objects here.

void setup() {
  Serial.begin(9600);
  
  // Set up I2C for the Arduino as a slave.
  // It will listen for commands from the NodeMCU.
  Wire.begin(I2C_SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent); // Register function to call when data is received
  Wire.onRequest(requestEvent); // Register function to call when data is requested

  // Initialize all motors to a stopped state with a default speed
  // A speed of 200 is 78% of full throttle. You can change this later with PWM.
  leftMotors.setSpeed(200);
  rightMotors.setSpeed(200);
  seedSowing.setSpeed(60);
  waterPump.setSpeed(255);
  
  // Initially, stop all motors.
  stopAllMotors();
  
  Serial.println("Arduino ready to receive commands from NodeMCU.");
}

void loop() {
  // The loop is simple; we just handle the received command.
  switch (command) {
    case 'F': // Forward
      leftMotors.run(FORWARD);
      rightMotors.run(FORWARD);
      break;
    case 'B': // Backward
      leftMotors.run(BACKWARD);
      rightMotors.run(BACKWARD);
      break;
    case 'L': // Turn Left
      leftMotors.run(BACKWARD); // Left motors backward
      rightMotors.run(FORWARD); // Right motors forward
      break;
    case 'R': // Turn Right
      leftMotors.run(FORWARD);  // Left motors forward
      rightMotors.run(BACKWARD); // Right motors backward
      break;
    case 'S': // Stop all movement
      stopAllMotors();
      break;
    case 'P': // Water Pump On
      waterPump.run(FORWARD);
      break;
    case 'O': // Water Pump Off
      waterPump.run(RELEASE);
      break;
    case 'D': // Seed Sowing On
      seedSowing.run(FORWARD);
      break;
    case 'C': // Seed Sowing Off
      seedSowing.run(RELEASE);
      break;
    case 'T': // Read soil sensor data
      // This command is handled in the requestEvent function below.
      break;
  }
}

// This function is called automatically when NodeMCU sends data to the Arduino.
void receiveEvent(int byteCount) {
  // Read the incoming command. We expect a single character.
  if (Wire.available()) {
    command = Wire.read();
    Serial.print("Received command: ");
    Serial.println(command);
  }
}

// This function is called automatically when NodeMCU requests data from the Arduino.
void requestEvent() {
  // --- SENSOR READING CODE GOES HERE ---
  // replace this placeholder with the actual code to read from 7-in-1 sensor.
  // For most 7-in-1 sensors, this involves sending a specific command and parsing the response.
  
  // This is a placeholder that generates dummy values for demonstration.
  soilMoisture = map(analogRead(A0), 0, 1023, 0, 100);
  soilTemperature = map(analogRead(A0), 0, 1023, 15, 30);
  soilPH = map(analogRead(A0), 0, 1023, 5, 8);
  soilNitrogen = map(analogRead(A0), 0, 1023, 10, 50);
  soilPhosphorus = map(analogRead(A0), 0, 1023, 10, 50);
  soilPotassium = map(analogRead(A0), 0, 1023, 10, 50);

  // You would replace the above dummy code with something like this:
  // mySensor.read(); // Read all values from the sensor
  // soilMoisture = mySensor.getMoisture();
  // soilTemperature = mySensor.getTemperature();
  // etc.

  // Prepare the data to send back.
  // We'll format it as a single string for simplicity.
  String response = "Moisture: " + String(soilMoisture, 0) + "%\n" +
                    "Temp: " + String(soilTemperature, 1) + "C\n" +
                    "pH: " + String(soilPH, 1) + "\n" +
                    "N: " + String(soilNitrogen, 0) + " mg/kg\n" +
                    "P: " + String(soilPhosphorus, 0) + " mg/kg\n" +
                    "K: " + String(soilPotassium, 0) + " mg/kg";
  
  // Convert the String to a char array and send it.
  char responseBuffer[200];
  response.toCharArray(responseBuffer, 200);
  Wire.write(responseBuffer);
}

// Helper function to stop all motors
void stopAllMotors() {
  leftMotors.run(RELEASE);
  rightMotors.run(RELEASE);
  seedSowing.run(RELEASE);
  waterPump.run(RELEASE);
}
