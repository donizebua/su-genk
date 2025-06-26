// CCW

#include <Wire.h>

// ======= Motor Pin ==========
const int IN1 = 3;  // Motor kiri PWM
const int IN2 = 2;  // Motor kiri LOW
const int IN3 = 5;  // Motor kanan PWM
const int IN4 = 4;  // Motor kanan LOW

// ======= IMU MPU6050 =========
float RateYaw = 0, AngleYaw = 0;
float YawOffset = 0;
unsigned long prevTime = 0;

// ======= Logika Belok =========
bool belokKanan = true; // kondisi awal: belok ke kanan
unsigned long delayStart = 0;
bool sedangDelay = false;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(100);

  // Setup pin motor
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopMotor();

  // Inisialisasi MPU6050
  Wire.beginTransmission(0x69);
  Wire.write(0x6B); // Power management register
  Wire.write(0x00); // Wake up
  Wire.endTransmission();

  // Set sensitivitas gyro ±500°/s (FS_SEL = 1)
  Wire.beginTransmission(0x69);
  Wire.write(0x1B); // Gyro config
  Wire.write(0x08); // ±500 dps
  Wire.endTransmission();

  // === Kalibrasi Selama 2 Detik ===
  Serial.println("Kalibrasi IMU...");
  for (int i = 0; i < 2000; i++) {
    bacaGyroZ();
    YawOffset += RateYaw;
    delay(1);
  }
  YawOffset /= 2000.0;
  Serial.println("Selesai kalibrasi.");

  prevTime = millis();
}

void loop() {
  updateYaw();

  Serial.print("Yaw: ");
  Serial.println(AngleYaw);

  if (sedangDelay) {
    if (millis() - delayStart >= 2000) {
      sedangDelay = false;
      belokKanan = !belokKanan; // Ganti arah
    } else {
      stopMotor();
      return;
    }
  }

  if (belokKanan) {
    // Belok kanan hingga mencapai 270 derajat (dari 0 naik CCW)
    if (AngleYaw < 260 || AngleYaw > 280) {
      belokKanan90();
    } else {
      stopMotor();
      sedangDelay = true;
      delayStart = millis();
    }
  } else {
    // Belok kiri kembali ke 0 dari 270 (counter-clockwise)
    if (AngleYaw > 10 && AngleYaw < 350) {
      belokKiri90();
    } else {
      stopMotor();
      sedangDelay = true;
      delayStart = millis();
    }
  }

  delay(10);
}

// ======================= Fungsi IMU =======================
void bacaGyroZ() {
  Wire.beginTransmission(0x69);
  Wire.write(0x43); // GYRO_ZOUT_H
  Wire.endTransmission();
  Wire.requestFrom(0x69, 2);
  int16_t gyroZ = Wire.read() << 8 | Wire.read();
  RateYaw = (float)gyroZ / 65.5; // untuk ±500°/s
}

void updateYaw() {
  bacaGyroZ();
  RateYaw -= YawOffset;

  unsigned long currTime = millis();
  float dt = (currTime - prevTime) / 1000.0;
  prevTime = currTime;

  AngleYaw += RateYaw * dt;

  // Normalisasi agar tetap dalam range [0, 360)
  if (AngleYaw >= 360) AngleYaw -= 360;
  if (AngleYaw < 0) AngleYaw += 360;
}

// ======================= Fungsi Motor =======================
void stopMotor() {
  analogWrite(IN1, 0); digitalWrite(IN2, LOW);
  analogWrite(IN3, 0); digitalWrite(IN4, LOW);
}

void belokKanan90() {
  analogWrite(IN1, 110); digitalWrite(IN2, LOW);  // kiri maju
  analogWrite(IN4, 120); digitalWrite(IN3, LOW);  // kanan mundur
}

void belokKiri90() {
  analogWrite(IN2, 110); digitalWrite(IN1, LOW);  // kiri mundur
  analogWrite(IN3, 120); digitalWrite(IN4, LOW);  // kanan maju
}
