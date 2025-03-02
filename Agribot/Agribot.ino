

// Define pin connections
const int humidity = 2;
const int soilMoisturePin = A0;
const int obstacleAvoidancePin = ---;//ultrasonic sensor pin
const int soilTemperaturePin = D2;
const int rainSensorPin = 4; // Pin connected to the rainwater sensor

//single motor 2 pin declaration --> 4 wheel motor
const int motor1A = D1;
const int motor1B = D1;
const int motor2A = D3;
const int motor2B = D3;
const int motor3A = D5;
const int motor3B = D5;
const int motor4A = D6;
const int motor4B = D6;

// Define trigger distance for obstacle detection
const int triggerDistance = 10; // cm

// Define motor speed values
const int forwardSpeed = 255;
const int turnSpeed = 100;

// Create objects for sensors
Ultrasonic sensor(obstacleAvoidancePin);
dhtSensor = new DHT(DHT11, soilTemperaturePin); 

// Global variables
int obstacleDistance = 0;
float soilMoisture = 0.0;
float soilTemperature = 0.0;

void setup() {
  Serial.begin(115200); // Initialize serial communication for debugging
  // Start DHT sensor
  sensor.begin();
  //seedSowerServo.attach(servoPin);
  pinMode(rainSensorPin, rainSensorPin == A0 ? INPUT_ANALOG : INPUT); // Set rain sensor pin mode 
}

void loop() {
  moveForward();
  int rainSensorValue = analogRead(rainSensorPin); // Read analog value from rain sensor 
  bool isRaining = rainSensorValue > 500; // Example threshold for detecting rain 

  // Read obstacle distance
  obstacleDistance = sensor.readDistanceCm();
  // Check for obstacle and take action (replace with your desired logic)
  if (obstacleDetected(obstacleDistance)) {
    Serial.println("Obstacle detected!");
     stopMotors();
     moveBackward();
     delay(1000);
     turnRight();
     delay(1000);
  }
  // Read soil moisture 
  soilMoisture = analogRead(soilMoisturePin) / 1023.0;
  // Read soil temperature and humidity from DHT sensor
  sensor.readTemperature(&soilTemperature, &humidity);
  // Print sensor data to serial monitor
  Serial.print("Obstacle distance (cm): ");
  Serial.println(obstacleDistance);
  Serial.print("Soil moisture: ");
  Serial.println(soilMoisture);
  Serial.print("Soil temperature (°C): ");
  Serial.println(soilTemperature);
  Serial.print("Humidity (%): ");
  Serial.println(humidity);
  // Delay between readings
  delay(1000);
}

//wheel motors
// Function to stop the robot --> set both motors to brake
void stopMotors() {
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, LOW);
  digitalWrite(motor3A, LOW);
  digitalWrite(motor3B, LOW);
  digitalWrite(motor4A, LOW);
  digitalWrite(motor4B, LOW);
}

// Function to move forward --> set both motors to forward
void moveForward() {
  digitalWrite(motor1A, HIGH);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, HIGH);
  digitalWrite(motor2B, LOW);
  digitalWrite(motor3A, HIGH);
  digitalWrite(motor3B, LOW);
  digitalWrite(motor4A, HIGH);
  digitalWrite(motor4B, LOW);
}

// Function to turn right --> set left motor forward, right motor backward
void turnRight() {
  digitalWrite(motor1A, HIGH);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, HIGH);
  digitalWrite(motor3A, HIGH);
  digitalWrite(motor3B, LOW);
  digitalWrite(motor4A, LOW);
  digitalWrite(motor4B, HIGH);
}

// Function to turn left--> set left motor backward, right motor forward
void turnLeft() {
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, HIGH);
  digitalWrite(motor2A, HIGH);
  digitalWrite(motor2B, LOW);
  digitalWrite(motor3A, LOW);
  digitalWrite(motor3B, HIGH);
  digitalWrite(motor4A, HIGH);
  digitalWrite(motor4B, LOW);
}

//may include leaf disease detection leafdetection.py, foreign object detection, camera module

//pest detection
#define servoPin 9
#define trigPin 13
#define echoPin 12

// create serve object
Servo serve;

// define variables for ultrasonic sensor
long duration;
int distance;

void setup() (
// initialize servo and ultrasonic sensor
servo.attach(servoPin);
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);

// initialize WiFi and camera

WiFi.begin(ssid, password);
while (WiFi.status() != WL CONNECTED) {
delay(1000);
}
camera.init();

void loop(){
  //detect distance using ultrasonic sensor
digitalWrite(trigPin, LOW);
delayMicrosecond(2);
digitalWrite(trigPin, HIGH);
delayMicrosecond(10);
duration = pluseIn(echoPin,HIGH);
distance = duration *0.034/2;
//rotate servo based on distance
if(distance<=30)
  servo.write(0);
else if(distance >30 && distance <=60)
  servo.write(90);
else if(distance > 60)
  servo.write(180);

//capture image from cam
camera.capture();
