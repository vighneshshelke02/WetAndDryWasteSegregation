#include <Servo.h>
Servo servo1;

const int trigPin = 12;
const int echoPin = 11;
const int potPin = A0;

long duration;
int distance = 0;
int soil = 0;
int fsoil = 0;

int maxDryValue = 1;       // Moisture % threshold
int Ultra_Distance = 25;   // Max trigger distance
int Min_Distance = 5;      // Ignore anything closer than this

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  servo1.attach(8);
  servo1.write(80); // New home position
  Serial.println("Soil Sensor     Ultrasonic          Servo");
}

void loop() {
  distance = getDistance();

  if (distance >= Min_Distance && distance <= Ultra_Distance) {
    Serial.print("Detected object at: ");
    Serial.print(distance);
    Serial.println(" cm");

    delay(500);  // Wait for object to settle

    int moisture = getSoilMoisture();

    Serial.print("Humidity: ");
    Serial.print(moisture);
    Serial.print("%    Distance: ");
    Serial.print(distance);
    Serial.print(" cm");

    if (moisture > maxDryValue) {
      Serial.println("     ==> WET Waste");
      safeServoMove(80, 160);
    } else {
      Serial.println("     ==> DRY Waste");
      safeServoMove(80, 10);
    }

    delay(1000);
    safeServoMove(servo1.read(), 80);
    delay(1500); // Cooldown time
  }

  distance = 0;
  fsoil = 0;
  delay(100); // Stable loop
}

// Distance averaging
int getDistance() {
  int dist = 0;
  for (int i = 0; i < 2; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH, 20000UL); // Timeout added
    dist += duration * 0.034 / 2;
    delay(5);
  }
  return dist / 2;
}

// Moisture average
int getSoilMoisture() {
  int total = 0;
  for (int i = 0; i < 3; i++) {
    soil = analogRead(potPin);
    soil = constrain(soil, 485, 1023);
    total += map(soil, 485, 1023, 100, 0);
    delay(40);
  }
  return total / 3;
}

// Smooth servo movement
void safeServoMove(int fromAngle, int toAngle) {
  int step = (toAngle > fromAngle) ? 1 : -1;
  for (int pos = fromAngle; pos != toAngle; pos += step) {
    servo1.write(pos);
    delay(10);
  }
  servo1.write(toAngle);
}
