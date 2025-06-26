// Ini merupakan kode utama yang di upload pada mikrokontroler.

// Robot menggunakan dua motor DC yang dikendalikan oleh Arduino dan sensor IMU MPU6050 untuk navigasi arah.

// Sensor IMU dikalibrasi saat awal menyala, sehingga sudut awal dianggap sebagai 0° dan digunakan sebagai referensi utama untuk arah lurus.

// Robot bergerak lurus sambil menjaga posisinya tetap di tengah antara dua dinding menggunakan kombinasi sensor IMU dan tiga sensor ultrasonik HC-SR04 (kiri, kanan, dan depan).

// Proses centering dilakukan dengan menyesuaikan PWM motor kiri dan kanan berdasarkan data sudut dari IMU dan jarak dari sensor ultrasonik.

// Sudut dari IMU digunakan untuk mendeteksi apakah robot melenceng ke kiri atau ke kanan dari arah 0°, lalu menghitung perubahan PWM motor untuk mengoreksi arah.

// Jika sudut berada pada domain kiri (1–179°), maka PWM motor kiri ditambah agar robot mengarah kembali ke kanan (lurus).

// Jika sudut berada pada domain kanan (181–359°), maka PWM motor kanan ditambah agar robot mengarah kembali ke kiri (lurus).

// Perubahan PWM akibat sudut dihitung menggunakan rumus changePWM = |sudut saat ini - titik referensi| × konstanta, dan konstanta ini bisa diatur secara manual.

// Sensor ultrasonik digunakan untuk menjaga jarak robot dari dinding kiri dan kanan, khususnya jika terlalu dekat.

// Jika jarak dari salah satu sisi kurang dari offset tertentu (misalnya 15 cm), maka PWM motor di sisi tersebut akan ditambah untuk menjauhkan robot dari dinding.

// Perubahan PWM akibat jarak juga dihitung menggunakan rumus changePWM = |offset - jarak terbaca| × konstanta, dan memiliki konstanta terpisah dari konstanta sudut.

// Total PWM akhir untuk masing-masing motor dihitung dari PWM dasar + perubahan akibat sudut + perubahan akibat jarak.

// Jika sensor depan mendeteksi dinding dalam jarak ≤ 15 cm, robot akan berhenti selama 1 detik untuk melakukan analisis belokan.

// Saat berhenti, robot membandingkan jarak dari sensor kiri dan kanan untuk menentukan arah jalur terbuka.

// Jika jalur lebih terbuka di kanan, robot akan berbelok kanan hingga mencapai sudut 270° berdasarkan IMU.

// Jika jalur lebih terbuka di kiri, robot tetap akan berbelok ke kanan hingga mencapai sudut 90° karena keterbatasan mekanik pada arah belok.

// Selama proses belok, hanya IMU yang digunakan sebagai acuan, sedangkan sensor jarak tidak aktif untuk centering.

// Setelah selesai belok, robot akan berhenti selama 1 detik lalu mereset sudut IMU kembali ke 0° sebagai referensi baru.

// Robot kemudian melanjutkan kembali pergerakan lurus dan proses centering, dan siklus akan berulang saat bertemu dinding lagi.

// Semua konstanta untuk perubahan PWM berdasarkan sudut dan jarak serta nilai offset dapat dikalibrasi untuk penyesuaian performa robot.

#include <Wire.h> 

// ======= Motor Pin ==========
const int IN1 = 3;
const int IN2 = 2;
const int IN3 = 5;
const int IN4 = 4;

// ======= Sonar Pin ==========
const int TRIG_DEPAN = 30;
const int ECHO_DEPAN = 28;
const int TRIG_KIRI = 24;
const int ECHO_KIRI = 22;
const int TRIG_KANAN = 36;
const int ECHO_KANAN = 34;

// ======= Flame Sensor Pin ==========
const int FLAME_1 = 41;
const int FLAME_2 = 43;
const int FLAME_3 = 45;
const int FLAME_4 = 47;
const int FLAME_5 = 49;

// ======= Kipas ==========
const int KIPAS_PWM = 9;

// ======= IMU ==========
float RateYaw, AngleYaw = 0;
float YawOffset = 0;
unsigned long prevTime = 0;

// ======= Konstanta Navigasi ==========
float konstantaSudutKanan = 6.0;  // nilai-nilai yang perlu diubah untuk kalibrasi
float konstantaSudutKiri = 10.0;
float konstantaSonarKanan = 5.0;
float konstantaSonarKiri = 5.0;

int pwmKiriDasar = 100;
int pwmKananDasar = 110;

int offsetSonar = 6;             // cm
int toleransiSudut = 15;         // derajat
int jarakDindingDepan = 18;      // cm

// ======= Navigasi Status ==========
bool sedangDelaySebelumBelok = false;
bool sedangDelaySetelahBelok = false;
bool sedangBelok = false;

unsigned long delayStart = 0;
float targetSudut = 0;

float jarakDepan, jarakKiri, jarakKanan;

// ======= Mode Robot ==========
enum ModeRobot { MAJU, BELAK, PADAMKAN_API };
ModeRobot mode = MAJU;
unsigned long waktuPadam = 0;
const unsigned long durasiPadam = 5000;

// ======= IMU ==========
void gyro_signals() {
  Wire.beginTransmission(0x69);
  Wire.write(0x43);
  Wire.endTransmission();
  Wire.requestFrom(0x69, 2);
  int16_t GyroZ = Wire.read() << 8 | Wire.read();
  RateYaw = (float)GyroZ / 65.5;
}

void updateYaw() {
  gyro_signals();
  RateYaw -= YawOffset;

  unsigned long currTime = millis();
  float dt = (currTime - prevTime) / 1000.0;
  prevTime = currTime;

  AngleYaw += RateYaw * dt;

  if (AngleYaw >= 360) AngleYaw -= 360;
  if (AngleYaw < 0) AngleYaw += 360;
}

void resetSudut() {
  AngleYaw = 0;
  Serial.println("Sudut IMU direset ke 0");
}

// ======= Sonar ==========
float bacaSonar(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  return (duration * 0.034) / 2;
}

void bacaSemuaSonar() {
  jarakDepan = bacaSonar(TRIG_DEPAN, ECHO_DEPAN);
  jarakKiri = bacaSonar(TRIG_KIRI, ECHO_KIRI);
  jarakKanan = bacaSonar(TRIG_KANAN, ECHO_KANAN);
}

// ======= Motor ==========
void stopMotor() {
  analogWrite(IN1, 0); analogWrite(IN2, 0);
  analogWrite(IN3, 0); analogWrite(IN4, 0);
}

void belokKanan() {
  analogWrite(IN1, 100); analogWrite(IN2, 0);
  analogWrite(IN3, 0);  analogWrite(IN4, 100);
}

void jalanLurus() {
  float changePWMSudutKiri = 0, changePWMSudutKanan = 0;

  if (AngleYaw >= 1 && AngleYaw <= 179)
    changePWMSudutKiri = abs(AngleYaw) * konstantaSudutKiri;
  else if (AngleYaw >= 181 && AngleYaw <= 359)
    changePWMSudutKanan = abs(AngleYaw - 360) * konstantaSudutKanan;

  float changePWMJarakKiri = 0, changePWMJarakKanan = 0;
  if (jarakKiri < offsetSonar)
    changePWMJarakKiri = abs(round(offsetSonar - jarakKiri)) * konstantaSonarKiri;
  if (jarakKanan < offsetSonar)
    changePWMJarakKanan = abs(round(offsetSonar - jarakKanan)) * konstantaSonarKanan;

  int pwmKiri = pwmKiriDasar + changePWMSudutKiri + changePWMJarakKiri - changePWMSudutKanan; // complementary filter
  int pwmKanan = pwmKananDasar + changePWMSudutKanan + changePWMJarakKanan - changePWMSudutKiri;

  pwmKiri = constrain(pwmKiri, 0, 255);
  pwmKanan = constrain(pwmKanan, 0, 255);

  analogWrite(IN1, pwmKiri); analogWrite(IN2, 0);
  analogWrite(IN3, pwmKanan); analogWrite(IN4, 0);
}

// ======= Sudut Target ==========
bool sudahSampaiTarget(float target) {
  float atas = target + toleransiSudut;
  float bawah = target - toleransiSudut;

  if (bawah < 0) bawah += 360;
  if (atas >= 360) atas -= 360;

  if (bawah < atas)
    return (AngleYaw >= bawah && AngleYaw <= atas);
  else
    return (AngleYaw >= bawah || AngleYaw <= atas);
}

// ======= Setup ==========
void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(100);

  pinMode(KIPAS_PWM, OUTPUT);
  pinMode(FLAME_1, INPUT);
  pinMode(FLAME_2, INPUT);
  pinMode(FLAME_3, INPUT);
  pinMode(FLAME_4, INPUT);
  pinMode(FLAME_5, INPUT);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  stopMotor();

  pinMode(TRIG_DEPAN, OUTPUT); pinMode(ECHO_DEPAN, INPUT);
  pinMode(TRIG_KIRI, OUTPUT);  pinMode(ECHO_KIRI, INPUT);
  pinMode(TRIG_KANAN, OUTPUT); pinMode(ECHO_KANAN, INPUT);

  Wire.beginTransmission(0x69); Wire.write(0x6B); Wire.write(0x00); Wire.endTransmission();
  Wire.beginTransmission(0x69); Wire.write(0x1B); Wire.write(0x08); Wire.endTransmission();

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

// ======= Loop ==========
void loop() {
  updateYaw();
  bacaSemuaSonar();

  // ==== Cek Flame ====
  bool api = digitalRead(FLAME_1) || digitalRead(FLAME_2) || digitalRead(FLAME_3) || digitalRead(FLAME_4) || digitalRead(FLAME_5);

  if (mode != PADAMKAN_API && api) {
    stopMotor();
    analogWrite(KIPAS_PWM, 200);
    waktuPadam = millis();
    mode = PADAMKAN_API;
    Serial.println("Api terdeteksi, mulai padamkan...");
    return;
  }

  if (mode == PADAMKAN_API) {
    if (millis() - waktuPadam >= durasiPadam) {
      analogWrite(KIPAS_PWM, 0);
      Serial.println("Api berhasil dipadamkan, lanjut navigasi");
      mode = MAJU;
    } else {
      return; // Tunggu sampai kipas selesai
    }
  }

  // ==== Navigasi ====
  if (mode == MAJU) {

    if (sedangDelaySebelumBelok) {
      if (millis() - delayStart >= 1000) {
        sedangDelaySebelumBelok = false;
        sedangBelok = true;
        Serial.print("Selesai delay, mulai belok ke "); Serial.println(targetSudut);
      }
      stopMotor();
      return;
    }

    if (sedangDelaySetelahBelok) {
      if (millis() - delayStart >= 1000) {
        sedangDelaySetelahBelok = false;
        resetSudut();
        Serial.println("Selesai delay, mulai jalan lurus");
      }
      stopMotor();
      return;
    }

    if (sedangBelok) {
      if (sudahSampaiTarget(targetSudut)) {
        stopMotor();
        sedangBelok = false;
        sedangDelaySetelahBelok = true;
        delayStart = millis();
        Serial.print("Sudut "); Serial.print(targetSudut); Serial.println(" tercapai, delay 1 detik");
        return;
      }
      belokKanan();
      return;
    }

    if (jarakDepan <= jarakDindingDepan) {
      stopMotor();
      sedangDelaySebelumBelok = true;
      delayStart = millis();
      Serial.println("Deteksi dinding depan, berhenti 1 detik sebelum belok");
      delay(100);
      bacaSemuaSonar();
      if (jarakKanan > jarakKiri) {
        targetSudut = 270;
        Serial.println("Belok ke kanan (270)");
      } else {
        targetSudut = 90;
        Serial.println("Belok ke kiri (90)");
      }
      return;
    }

    jalanLurus();
  }

  delay(10);
}
