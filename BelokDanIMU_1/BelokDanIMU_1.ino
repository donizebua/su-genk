// Kode dibawah menggabungkan gerak belok mobil dan imu. Tujuannya yaitu melakukan kalibrasi pwm motor untuk belok dan toleransi sudut imu yang sesuai.

#include <Wire.h>

// ======= Motor Pin ==========
const int IN1 = 3;  // Motor kiri PWM
const int IN2 = 2;  // Motor kiri LOW
const int IN3 = 5;  // Motor kanan PWM
const int IN4 = 4;  // Motor kanan LOW

// ======= IMU MPU6050 =========
float RateYaw, AngleYaw = 0;
float YawOffset = 0;
unsigned long prevTime = 0;

// ======= Logika Belok =========
bool sedangDelay = false;
bool stop0 = false;
bool stop180 = false;
unsigned long delayStart = 0;

// ======================= Fungsi IMU =======================
void gyro_signals() {

  Wire.beginTransmission(0x69);
  Wire.write(0x43); // Mulai dari GYRO_ZOUT_H
  Wire.endTransmission();
  Wire.requestFrom(0x69, 2);
  int16_t GyroZ = Wire.read() << 8 | Wire.read();
  RateYaw = (float)GyroZ / 65.5; // Untuk ±500°/s. Jika belum set, ubah ke 131.0

}

// ======================= Fungsi Motor =======================
void stopMotor() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}

// ====================== Belok Kanan ========================
void belokKanan() {
  // Belok Kanan (kedua gerak) sesuai instruksi
  analogWrite(IN1, 80);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 80);
}

// ==================== Update Yaw  =========================
void updateYaw() {

  gyro_signals();
  RateYaw -= YawOffset;

  unsigned long currTime = millis();
  float dt = (currTime - prevTime) / 1000.0;
  prevTime = currTime;

  AngleYaw += RateYaw * dt;

  // Normalisasi ke 0–359
  if (AngleYaw >= 360) AngleYaw -= 360;
  if (AngleYaw < 0) AngleYaw += 360;

  Serial.print("Yaw Angle = ");
  Serial.println(AngleYaw);

}


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
  Wire.beginTransmission(0x69); // ganti ke 0x68 apabila menggunakan AD0 LOW
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
    gyro_signals();
    YawOffset += RateYaw;
    delay(1);
  }
  YawOffset /= 2000;
  Serial.println("Selesai kalibrasi.");

  prevTime = millis();
}

void loop() {

  updateYaw();

  // Cek apakah sedang delay 2 detik
  if (sedangDelay) {
    if (millis() - delayStart >= 2000) {
      sedangDelay = false;
    }
    stopMotor();
    return;
  }

  // Cek kondisi stop di 180 derajat (170-190)
  if (AngleYaw >= 170 && AngleYaw <= 190 && !stop180) {  // toleransi sudut +- 10 derajat
    if (!sedangDelay) {
      stopMotor();
      stop180 =  true;
      stop0 = false;
      sedangDelay = true;
      delayStart = millis();
      Serial.println("Stop di 180 derajat selama 2 detik");
    }
    return;
  }

  // Cek kondisi stop di 0 derajat (350-360 atau 0-10)
  if ((AngleYaw >= 350 || AngleYaw <= 10) && !stop0) {
    if (!sedangDelay) {
      stopMotor();
      stop0 = true;
      stop180 = false;
      sedangDelay = true;
      delayStart = millis();
      Serial.println("Stop di 0 derajat selama 2 detik");
    }
    return;
  }

  // Default: terus belok kanan
  belokKanan();

  delay(10);
}

