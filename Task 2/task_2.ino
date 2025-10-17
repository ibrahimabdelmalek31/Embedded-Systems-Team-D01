enum Status {green, yellow, red, pgreen, pred}; // Traffic and pedestrian states
Status status = green;

unsigned long lastMillis = 0;  // Tracks the last state update
unsigned long interval = 10000; // Default time interval
bool pedestrianRequest = false; // Tracks if the pedestrian button is pressed

void setup() {
  // Traffic light pins
  pinMode(12, OUTPUT); // Pedestrian red light
  pinMode(11, OUTPUT); // Pedestrian green light
  pinMode(10, INPUT);  // Pedestrian button input
  pinMode(6, OUTPUT);  // Traffic red light
  pinMode(4, OUTPUT);  // Traffic yellow light
  pinMode(2, OUTPUT);  // Traffic green light (and pedestrian wait indicator)
}

void loop() {
  unsigned long currentMillis = millis();

  int buttonState = digitalRead(10); // Read pedestrian button state
  if (buttonState == HIGH && !pedestrianRequest && status != pgreen) {
    pedestrianRequest = true;         // Set pedestrian request
    lastMillis = currentMillis;       // Reset timing
    status = pgreen;                  // Transition to pedestrian green state
    interval = 3200;                  // Time to transition traffic lights for pedestrians
  }

  if (pedestrianRequest) {
    handlePedestrianLights(currentMillis); // Manage pedestrian lights
  } else {
    handleTrafficLights(currentMillis); // Manage traffic lights
  }
}

void handleTrafficLights(unsigned long currentMillis) {
  switch (status) {
    case green:
      if (digitalRead(2) == LOW) {
        digitalWrite(2, HIGH); // Turn on traffic green light
        digitalWrite(12, HIGH); // Pedestrian red light ON
        lastMillis = currentMillis; // Reset timing
      }
      if (currentMillis - lastMillis >= interval) {
        digitalWrite(2, LOW); // Turn off traffic green light
        status = yellow;      // Transition to yellow state
        lastMillis = currentMillis;
        interval = 3000;      // Yellow light duration
      }
      break;

    case yellow:
      if (digitalRead(4) == LOW) {
        digitalWrite(4, HIGH); // Turn on traffic yellow light
        lastMillis = currentMillis;
      }
      if (currentMillis - lastMillis >= interval) {
        digitalWrite(4, LOW); // Turn off traffic yellow light
        status = red;         // Transition to red state
        lastMillis = currentMillis;
        interval = 10000;     // Red light duration
      }
      break;

    case red:
      if (digitalRead(6) == LOW) {
        digitalWrite(6, HIGH); // Turn on traffic red light
        lastMillis = currentMillis;
      }
      if (currentMillis - lastMillis >= interval) {
        digitalWrite(6, LOW); // Turn off traffic red light
        status = green;       // Return to green state
        lastMillis = currentMillis;
        interval = 10000;     // Green light duration
      }
      break;

    default:
      break;
  }
}

void handlePedestrianLights(unsigned long currentMillis) {
  switch (status) {
    case pgreen:
      if (currentMillis - lastMillis >= interval) {
        digitalWrite(6, HIGH);  // Turn on traffic red light
        digitalWrite(2, LOW);   // Turn off traffic green light
        digitalWrite(4, LOW);   // Turn off traffic yellow light
        digitalWrite(12, LOW); // Turn off pedestrian red light
        digitalWrite(11, HIGH); // Turn on pedestrian green light
        lastMillis = currentMillis;
        interval = 10000;       // Pedestrian green light duration
        status = pred;          // Transition to pedestrian red
      }
      break;

    case pred:
      if (currentMillis - lastMillis >= interval) {
        digitalWrite(11, LOW);  // Turn off pedestrian green light
        digitalWrite(12, HIGH); // Turn on pedestrian red light
        pedestrianRequest = false; // Reset pedestrian request
        status = green;         // Return to normal traffic light control
        lastMillis = currentMillis;
        interval = 10000;       // Reset interval for green
      }
      break;

    default:
      break;
  }
}
