
#include <Wire.h>

// C++ code
volatile boolean pushButton;
int redState = 0; 
enum State { red, yellow, green, pedestrianGreen, pedestrianRed };
State _currentState = green;
State _targetState = pedestrianRed;
unsigned long time = 5000; // Default time for each state
unsigned long previousMillis = 0;  // Store last time state was updated
unsigned long pedestrianMillis = 0; // Store pedestrian light time
int set;
const int address = 4;

void setup() {
  Wire.begin();
  
  // Pin assignments
  pinMode(4, OUTPUT);  // Pedestrian red light
  pinMode(5, OUTPUT);  // Pedestrian green light
  pinMode(9, OUTPUT);  // Traffic red light
  pinMode(10, OUTPUT); // Traffic yellow light
  pinMode(11, OUTPUT); // Traffic green light
  pinMode(2, INPUT);   // Button to trigger pedestrian light
  
  // Attach interrupt for button press (active low)
  attachInterrupt(digitalPinToInterrupt(2), buttonPressed, FALLING); 
}

void loop() {
  Wire.beginTransmission(4);
  Wire.endTransmission();

  unsigned long currentMillis = millis();

  if (redState == 1) {
    pedestrian(currentMillis); // Handle pedestrian light state
  } else {
    trafficLight(currentMillis); // Handle traffic light state
  }
}

void trafficLight(unsigned long currentMillis) {
  switch (_currentState) {
    case green:
      digitalWrite(11, HIGH);  // Traffic green light on
      digitalWrite(4, HIGH);   // Pedestrian red light on
      if (currentMillis - previousMillis >= time) {
        digitalWrite(11, LOW);   // Traffic green light off
        previousMillis = currentMillis;
        _currentState = yellow;  // Transition to yellow state
        set = 0;
      }
      break;

    case yellow:
      digitalWrite(10, HIGH);   // Traffic yellow light on
      if (currentMillis - previousMillis >= time / 2) {
        digitalWrite(10, LOW);    // Traffic yellow light off
        previousMillis = currentMillis;
        if (set == 0)
          _currentState = red;    // Transition to red if no pedestrian request
        else
          _currentState = green; // Transition back to green after pedestrian phase
      }
      break;

    case red:
      digitalWrite(9, HIGH);    // Traffic red light on
      if (currentMillis - previousMillis >= time) {
        digitalWrite(9, LOW);     // Traffic red light off
        previousMillis = currentMillis;
        if (redState == 0)        // If no pedestrian request
          _currentState = yellow; // Transition to yellow
        else
          _currentState = pedestrianGreen; // Transition to pedestrian green
        set = 1;
      }
      break;
  }
}

void pedestrian(unsigned long currentMillis) {
  digitalWrite(10, LOW); // Turn off traffic yellow light during pedestrian phase

  switch (_targetState) {
    case pedestrianGreen:
      digitalWrite(9, HIGH);   // Traffic red light on
      digitalWrite(5, HIGH);   // Pedestrian green light on
      if (currentMillis - pedestrianMillis >= time / 5) {
        digitalWrite(5, LOW);    // Pedestrian green light off
        pedestrianMillis = currentMillis;
        redState = 1;
        _targetState = yellow;  // Transition to yellow pedestrian light
      }
      break;

    case yellow:
      digitalWrite(9, HIGH);   // Traffic red light on
      digitalWrite(10, HIGH);  // Traffic yellow light on
      if (currentMillis - pedestrianMillis >= time / 2) {
        digitalWrite(10, LOW);   // Traffic yellow light off
        pedestrianMillis = currentMillis;
        _targetState = pedestrianRed;
      }
      break;

    case pedestrianRed:
      digitalWrite(9, LOW);    // Traffic red light off
      digitalWrite(4, HIGH);   // Pedestrian red light on (pin 4)
      if (currentMillis - pedestrianMillis >= time * 2) {
        digitalWrite(4, LOW);    // Pedestrian red light off
        redState = 0;            // Reset redState after pedestrian phase finishes
        _targetState = _currentState; // Return to the current traffic state
      }
      break;
  }
}

void buttonPressed() {
  // When the button is pressed, change the state for pedestrian light
  if (digitalRead(2) == LOW) {  // Button pressed (active LOW)
    redState = 1;               // Trigger pedestrian phase
    _targetState = pedestrianGreen; // Set to pedestrian green state
    previousMillis = millis();  // Reset the timing
  }
}

