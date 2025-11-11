// ----- ENUM & VARIABLES -----
enum Status {green, yellow, red, pgreen, pred};
Status status = green;

unsigned long lastMillis = 0;
unsigned long interval = 10000;
bool pedestrianRequest = false;

// ----- 7-SEGMENT SETUP -----
int segPins[7] = {7, 8, 9, A0, A1, A2, A3}; // Segment order: a,b,c,d,e,f,g

byte digits[10][7] = {
  {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1},
  {1,1,1,1,0,0,1}, {0,1,1,0,0,1,1}, {1,0,1,1,0,1,1},
  {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0}, {1,1,1,1,1,1,1},
  {1,1,1,1,0,1,1}
};

// ----- PROTOTYPES -----
void handleTrafficLights(unsigned long currentMillis);
void handlePedestrianLights(unsigned long currentMillis);
void displayDigit(int num);
void clearDisplay();
void displayCountdown(unsigned long currentMillis);

// ----- SETUP -----
void setup() {
  pinMode(12, OUTPUT); // Ped red
  pinMode(11, OUTPUT); // Ped green
  pinMode(10, INPUT);  // Button
  pinMode(6, OUTPUT);  // Traffic red
  pinMode(4, OUTPUT);  // Traffic yellow
  pinMode(2, OUTPUT);  // Traffic green

  for (int i = 0; i < 7; i++) pinMode(segPins[i], OUTPUT);

  // Initial states: traffic green, ped red
  digitalWrite(2, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(6, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, HIGH);

  lastMillis = millis();
}

// ----- LOOP -----
void loop() {
  unsigned long currentMillis = millis();
  int buttonState = digitalRead(10);

  static unsigned long lastButtonPress = 0;
  if (buttonState == HIGH && !pedestrianRequest && status != pgreen &&
      currentMillis - lastButtonPress > 500) {
    pedestrianRequest = true;
    lastButtonPress = currentMillis;
    lastMillis = currentMillis;
    status = pgreen;
    interval = 3200;
  }

  if (pedestrianRequest)
    handlePedestrianLights(currentMillis);
  else
    handleTrafficLights(currentMillis);

  displayCountdown(currentMillis);
}

// ----- TRAFFIC LIGHT HANDLER -----
void handleTrafficLights(unsigned long currentMillis) {
  switch (status) {
    case green:
      digitalWrite(2, HIGH);  // traffic green
      digitalWrite(4, LOW);
      digitalWrite(6, LOW);
      digitalWrite(11, LOW);  // ped green
      digitalWrite(12, HIGH); // ped red
      if (currentMillis - lastMillis >= interval) {
        status = yellow;
        lastMillis = currentMillis;
        interval = 3000;
      }
      break;

    case yellow:
      digitalWrite(2, LOW);
      digitalWrite(4, HIGH);  // yellow
      digitalWrite(6, LOW);
      digitalWrite(11, LOW);
      digitalWrite(12, HIGH);
      if (currentMillis - lastMillis >= interval) {
        status = red;
        lastMillis = currentMillis;
        interval = 10000;
      }
      break;

    case red:
      digitalWrite(2, LOW);
      digitalWrite(4, LOW);
      digitalWrite(6, HIGH);  // traffic red
      digitalWrite(11, LOW);
      digitalWrite(12, HIGH); // ped red (normal)
      if (currentMillis - lastMillis >= interval) {
        status = green;
        lastMillis = currentMillis;
        interval = 10000;
      }
      break;
  }
}

// ----- PEDESTRIAN LIGHT HANDLER -----
void handlePedestrianLights(unsigned long currentMillis) {
  switch (status) {
    case pgreen:
      // traffic stops, pedestrians walk
      digitalWrite(2, LOW);
      digitalWrite(4, LOW);
      digitalWrite(6, HIGH);  // traffic red
      digitalWrite(11, HIGH); // ped green
      digitalWrite(12, LOW);  // ped red OFF
      if (currentMillis - lastMillis >= interval) {
        status = pred;
        lastMillis = currentMillis;
        interval = 10000;
      }
      break;

    case pred:
      // pedestrians stop, traffic resumes
      digitalWrite(11, LOW);
      digitalWrite(12, HIGH); // ped red
      digitalWrite(6, LOW);   // traffic red OFF
      digitalWrite(2, HIGH);  // traffic green ON
      if (currentMillis - lastMillis >= interval) {
        pedestrianRequest = false;
        status = green;
        lastMillis = currentMillis;
        interval = 10000;
      }
      break;
  }
}

// ----- 7-SEGMENT FUNCTIONS -----
void displayDigit(int num) {
  for (int i = 0; i < 7; i++)
    digitalWrite(segPins[i], digits[num][i]);
}

void clearDisplay() {
  for (int i = 0; i < 7; i++)
    digitalWrite(segPins[i], LOW);
}

void displayCountdown(unsigned long currentMillis) {
  int remaining = (interval - (currentMillis - lastMillis)) / 1000;
  if (remaining < 0) remaining = 0;
  if (remaining > 9) remaining = 9;

  static unsigned long lastBlink = 0;
  static bool blink = true;
  if (remaining == 0) {
    if (currentMillis - lastBlink >= 500) {
      blink = !blink;
      lastBlink = currentMillis;
    }
    if (blink)
      displayDigit(0);
    else
      clearDisplay();
  } else {
    displayDigit(remaining);
  }
}
