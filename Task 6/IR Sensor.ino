const int irPin = 2;

void setup() {
  pinMode(irPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  int irValue = digitalRead(irPin);

  Serial.print("IR Sensor Output: ");
  Serial.println(irValue);   // 0 or 1

  delay(200);
}
