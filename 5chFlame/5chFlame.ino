// Kode ini digunakan untuk mengkalibrasi sensitivitas  sensor flame agar jarak deteksi sesuai dengan yang diharapkan.

// Daftar pin input digital
const int pin1 = 41;
const int pin2 = 43;
const int pin3 = 45;
const int pin4 = 47;
const int pin5 = 49;

// Motor kiri
const int IN1 = 3;
const int IN2 = 2;

// Motor kanan
const int IN3 = 5;
const int IN4 = 4;


unsigned long curtime = 0;
unsigned long threeseconds = 3000;
unsigned long onesecond = 1000;
bool maju = true;

void setup() {
  // Set kelima pin sebagai input
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  pinMode(pin4, INPUT);
  pinMode(pin5, INPUT);

    // Set pin sebagai output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);

  // Mulai komunikasi serial
  Serial.begin(9600);
}

void loop() {
  unsigned long time = millis();
  // Baca status masing-masing pin
  int status1 = digitalRead(pin1);
  int status2 = digitalRead(pin2);
  int status3 = digitalRead(pin3);
  int status4 = digitalRead(pin4);
  int status5 = digitalRead(pin5);

  if (maju) {
    if (time - curtime < threeseconds){
      // Gerak maju
      analogWrite(IN1, 125);
      analogWrite(IN2, 0);
      analogWrite(IN3, 150);
      analogWrite(IN4, 0);
    } else {
      maju = false;
      curtime = time;

    }
  } else {
    if (time - curtime < onesecond){
      analogWrite(IN1, 0);
      analogWrite(IN2, 0);
      analogWrite(IN3, 0);
      analogWrite(IN4, 0);
    } else {
      maju = true;
      curtime = time;
    }
  }

  // Tampilkan ke Serial Monitor
  Serial.print("PIN 2: "); Serial.print(status1);
  Serial.print("    PIN 3: "); Serial.print(status2);
  Serial.print("    PIN 4: "); Serial.print(status3);
  Serial.print("    PIN 5: "); Serial.print(status4);
  Serial.print("    PIN 6: "); Serial.println(status5);

  delay(500); // jeda 500 ms untuk pembacaan berikutnya
}


