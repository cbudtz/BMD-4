#include <Arduino.h>

#include <Wire.h>
const int MPU = 0x68;
float AccX, AccY, AccZ, GyroX, GyroY, GyroZ, TargetAccX, TargetAccY, TargetAccZ, AccDiffX, AccDiffY, AccDiffZ, startTime, tuckCompleteTime;
bool tucking, tuckDone;

void readacc(float *accArray){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  accArray[0] = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  accArray[1] = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  accArray[2] = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  
}

void setup() {
  Serial.begin(19200);
  Serial.println("Starting...");
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);
  float acc[3];
  readacc(acc);
  TargetAccX = acc[0];
  TargetAccY = acc[1];
  TargetAccZ = acc[2];
  startTime = millis();
  tuckCompleteTime = 4294967040.0;
  tuckDone = false;
}

void loop() {
  Serial.println("Blinking...");
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(50);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);
  float acc[3];
  readacc(acc);
  Serial.print("AccX, Y, Z: ");
  Serial.print(acc[0]); Serial.print(", ");
  Serial.print(acc[1]);Serial.print(", ");
  Serial.print(acc[2]);Serial.println(", ");
  Serial.print("AccZ Diff: "); Serial.println(TargetAccZ-acc[2]);
  AccDiffZ = TargetAccZ-acc[2];
  AccDiffY = TargetAccY-acc[1];
  if (millis()-startTime > 5000){
    AccDiffZ = AccDiffZ +0.4;
  }
  if (millis()-tuckCompleteTime > 2000){
    AccDiffZ = AccDiffZ -0.4;
  }
  if (AccDiffY > 0.2) {
    digitalWrite(7, HIGH);
  } else {
    digitalWrite(7, LOW);
  }
   if (AccDiffY < -0.2) {
    digitalWrite(8, HIGH);
  } else {
    digitalWrite(8, LOW);
      
  }
  if (AccDiffZ > 0.2){
    digitalWrite(5, HIGH);
  } else {
    digitalWrite(5, LOW);
     if (millis()-startTime > 7000 && !tuckDone) {
          tuckCompleteTime = millis(); 
          tuckDone= true;
    }
  }
  if (AccDiffZ < -0.2) {
    digitalWrite(6,HIGH);
  } else {
    digitalWrite(6,LOW);
  }

  //Read some gyro data
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
    Serial.print("Gyro X, Y, Z: ");
  Serial.print(GyroX); Serial.print(", ");
  Serial.print(GyroY);Serial.print(", ");
  Serial.print(GyroZ);Serial.println(", ");
}

