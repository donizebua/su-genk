// Motor kiri
const int IN1 = 3;
const int IN2 = 2;

// Motor kanan
const int IN3 = 5;
const int IN4 = 4;

void setup() {
  // Set pin sebagai output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}

void loop() {
  // Gerak maju
  analogWrite(IN1, 125);
  analogWrite(IN2, 0);
  analogWrite(IN3, 150);
  analogWrite(IN4, 0);

  delay(3000);

  //Stop
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);

  delay(1000);

  // Gerak Mundur
  analogWrite(IN1, 0);
  analogWrite(IN2, 110);
  analogWrite(IN3, 0);
  analogWrite(IN4, 120);

  delay(3000);

  //Stop
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);

  delay(1000);

  // Belok Kanan
  analogWrite(IN1, 80);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);

  delay(1000);

  //Stop
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);

  delay(5000);

  // Belok Kiri
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 80);
  
  delay(1000);

  //Stop
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);

  delay(5000);

  analogWrite(IN1, 80);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 80);

  delay(1000);

  //Stop
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);

  delay(5000);

}