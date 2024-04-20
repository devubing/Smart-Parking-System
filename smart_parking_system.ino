#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Pin connections for ultrasonic sensor
const int trigPin = 2;
const int echoPin = 3;

// Pin connections for IR sensors
const int irSensor1Pin = 4;
const int irSensor2Pin = 5;
const int irSensor3Pin = 6;
const int irSensorExitPin = 7;

// Pin connections for servo motors
const int servoMotorParkingPin = 8;
const int servoMotorExitPin = 9;

// Variables to store parking slot states
bool slot1Occupied = false;
bool slot2Occupied = false;
bool slot3Occupied = false;

// Variables to store previous IR sensor states
bool prevSensor1State = false;
bool prevSensor2State = false;
bool prevSensor3State = false;
bool prevExitSensorState = false;e:

// Servo objects
Servo servoParking;
Servo servoExit;

// LCD configuration
LiquidCrystal_I2C lcd(0x3F, 16, 2);  // Change the address if necessary

bool lcdUpdateFlag = false;
bool gateClosed = true;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Parking");
  delay(2000);  // Display the message for 2 seconds
  lcd.clear();

  // Configure ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Configure IR sensor pins
  pinMode(irSensor1Pin, INPUT);
  pinMode(irSensor2Pin, INPUT);
  pinMode(irSensor3Pin, INPUT);
  pinMode(irSensorExitPin, INPUT);

  // Attach servo motors to their respective pins
  servoParking.attach(servoMotorParkingPin);
  servoExit.attach(servoMotorExitPin);

  // Set initial positions of servo motors
  servoParking.write(0);
  servoExit.write(90);
}

void loop() {
  // Read ultrasonic sensor to detect object at 5cm distance
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58;

  if (distance <= 5 && gateClosed) {
    // Object detected, rotate parking servo motor half rotation clockwise
    servoParking.write(90);
    delay(3000);  // Wait for 30 seconds
    servoParking.write(0);  // Return to initial position
    gateClosed = false;
  }
  
  // Check if the ultrasonic sensor no longer detects an object and close the gate
  if (distance > 5 && !gateClosed) {
    gateClosed = true;
  }

  // Read IR sensor states
  bool sensor1State = digitalRead(irSensor1Pin);
  bool sensor2State = digitalRead(irSensor2Pin);
  bool sensor3State = digitalRead(irSensor3Pin);
  bool exitSensorState = digitalRead(irSensorExitPin);

  // Check if the object is detected by the IR sensor for each slot
  if (sensor1State != prevSensor1State) {
    if (sensor1State == LOW) {
      slot1Occupied = true;
    } else {
      slot1Occupied = false;
    }
    lcdUpdateFlag = true;
  }

  if (sensor2State != prevSensor2State) {
    if (sensor2State == LOW) {
      slot2Occupied = true;
    } else {
      slot2Occupied = false;
    }
    lcdUpdateFlag = true;
  }

  if (sensor3State != prevSensor3State) {
    if (sensor3State == LOW) {
      slot3Occupied = true;
    } else {
      slot3Occupied = false;
    }
    lcdUpdateFlag = true;
  }

  // Check if the object is detected by the exit IR sensor
  if (exitSensorState != prevExitSensorState) {
    if (exitSensorState == LOW) {
      // Object detected, rotate exit servo motor half rotation anticlockwise
      servoExit.write(0);
      delay(2000); // Delay for 1 second
      servoExit.write(90); // Return to initial position
    }
  }

  // Store previous IR sensor states
  prevSensor1State = sensor1State;
  prevSensor2State = sensor2State;
  prevSensor3State = sensor3State;
  prevExitSensorState = exitSensorState;

  // Update LCD if the flag is set
  if (lcdUpdateFlag) {
    lcd.setCursor(0, 0);
    lcd.print("P1: ");
    lcd.print(slot1Occupied ? "Filled" : "Empty");

    lcd.setCursor(8, 0);
    lcd.print("P2: ");
    lcd.print(slot2Occupied ? "Filled" : "Empty");

    lcd.setCursor(0, 1);
    lcd.print("P3: ");
    lcd.print(slot3Occupied ? "Filled" : "Empty");

    lcdUpdateFlag = false;
  }
}