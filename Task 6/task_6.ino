enum State {GREEN, YELLOW, RED, PED_GREEN};
State state = GREEN;

unsigned long lastTime = 0;
bool pedRequest = false;

// Pins
const int irPin = 10;
const int trigPin = 8;
const int echoPin = 9;

const int tRed = 6;
const int tYellow = 4;
const int tGreen = 2;

const int pRed = 12;
const int pGreen = 11;

const float SAFE_DIST = 50.0;

float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH, 30000) * 0.034 / 2;
}

void setup() {
  pinMode(irPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(tRed, OUTPUT);
  pinMode(tYellow, OUTPUT);
  pinMode(tGreen, OUTPUT);
  pinMode(pRed, OUTPUT);
  pinMode(pGreen, OUTPUT);

  digitalWrite(tGreen, HIGH);
  digitalWrite(pRed, HIGH);
}

void loop() {
  unsigned long now = millis();
  float dist = getDistance();

  if (digitalRead(irPin) && dist > SAFE_DIST && !pedRequest) {
    pedRequest = true;
    state = YELLOW;
    lastTime = now;
  }

  switch (state) {

    case GREEN:
      break;

    case YELLOW:
      digitalWrite(tGreen, LOW);
      digitalWrite(tYellow, HIGH);
      if (now - lastTime > 3000) {
        digitalWrite(tYellow, LOW);
        digitalWrite(tRed, HIGH);
        state = PED_GREEN;
        lastTime = now;
      }
      break;

    case PED_GREEN:
      digitalWrite(pRed, LOW);
      digitalWrite(pGreen, HIGH);
      if (now - lastTime > 10000) {
        digitalWrite(pGreen, LOW);
        digitalWrite(pRed, HIGH);
        digitalWrite(tRed, LOW);
        digitalWrite(tGreen, HIGH);
        pedRequest = false;
        state = GREEN;
        lastTime = now;
      }
      break;
  }
}
