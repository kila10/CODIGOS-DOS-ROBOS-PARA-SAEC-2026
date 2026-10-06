#include <BluetoothSerial.h>
BluetoothSerial SerialBT;

// Motor control pins
const int IN1 = 25;
const int IN2 = 26;
const int IN3 = 27;
const int IN4 = 14;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Car"); // Bluetooth name
  Serial.println("Bluetooth Ready. Connect to 'ESP32_Car'");

  // connect ENA & ENB to 5V of ESP 32 board to move the car in maximum speed and to lower the speed connect it to 3.3V //

  // Set motor control pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  stopMotors();
}

void loop() {
  if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n'); // Read full string (e.g., "FR")
    command.trim(); // Remove spaces or \r\n
    Serial.println("Received: " + command);
    handleCommand(command);
  }
}

// Handle received command
void handleCommand(String cmd) {
  if (cmd == "F") forward();
  else if (cmd == "B") backward();
  else if (cmd == "L") left();
  else if (cmd == "R") right();
  else if (cmd == "FR") forwardRight();
  else if (cmd == "FL") forwardLeft();
  else if (cmd == "BR") backwardRight();
  else if (cmd == "BL") backwardLeft();
  else if (cmd == "S") stopMotors();
  else stopMotors(); // Unknown command
}

// Movement functions
void forward() {
  digitalWrite(IN1, LOW);   
  digitalWrite(IN2, HIGH);  
  digitalWrite(IN3, LOW);   
  digitalWrite(IN4, HIGH);  
}

void backward() {
  digitalWrite(IN1, HIGH);  
  digitalWrite(IN2, LOW);   
  digitalWrite(IN3, HIGH);  
  digitalWrite(IN4, LOW);   
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void forwardRight() {
  // Right motor slightly slower
  digitalWrite(IN1, LOW);   
  digitalWrite(IN2, HIGH);  
  digitalWrite(IN3, LOW);   
  digitalWrite(IN4, LOW);   // stop right motor → smooth right turn
}

void forwardLeft() {
  // Left motor slightly slower
  digitalWrite(IN1, LOW);   
  digitalWrite(IN2, LOW);   // stop left motor → smooth left turn
  digitalWrite(IN3, LOW);   
  digitalWrite(IN4, HIGH);  
}

void backwardRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);   // stop right motor
  digitalWrite(IN4, LOW);
}

void backwardLeft() {
  digitalWrite(IN1, LOW);   // stop left motor
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}