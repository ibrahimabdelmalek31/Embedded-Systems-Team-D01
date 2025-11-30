#include <Wire.h>

// C++ code
volatile boolean pushButton;
int redState = 1;  // Start in red state for traffic lights
enum State {red, yellow, green, pedestrianGreen, pedestrianRed};
State _currentState = green;
State _targetState = pedestrianGreen;
unsigned long time = 5000;
unsigned long previousMillis = 0;  // Used for timing (non-blocking)
unsigned long pedestrianMillis = 0;  // Used for pedestrian light timing
int set;
int event;

void setup() {
  Wire.begin(1);
  Wire.onReceive(receiveEvent);  // Set up Wire communication
  Serial.begin(9600);

  // Pin assignments for the lights
  pinMode(3, OUTPUT);  // Pedestrian red light
  pinMode(2, OUTPUT);  // Pedestrian green light
  pinMode(12, OUTPUT); // Traffic red light
  pinMode(11, OUTPUT); // Traffic yellow light
  pinMode(10, OUTPUT); // Traffic green light
  pinMode(5, INPUT);   // Button to trigger pedestrian light
  pinMode(4, OUTPUT);  // Traffic red light (alternate pin)
  
  // Attach interrupt for button press
  attachInterrupt(digitalPinToInterrupt(5), buttonPressed, RISING); 
}

void loop() {
  unsigned long currentMillis = millis();  // Get current time in milliseconds

  if (redState == 0) {
    pedestrian(currentMillis);  // Handle pedestrian light logic
  }
  if (redState == 1) {
    trafficLight(currentMillis);  // Handle traffic light logic
  }
}

void trafficLight(unsigned long currentMillis) {
  switch (_currentState) {
    case green:
      digitalWrite(10, HIGH);  // Traffic green light on
      digitalWrite(12, HIGH);   // Pedestrian red light on
      if (currentMillis - previousMillis >= time) {
        digitalWrite(10, LOW);  // Traffic green light off
        previousMillis = currentMillis;
        _currentState = yellow;  // Transition to yellow
        set = 0;
      }
      break;

    case yellow:
      digitalWrite(11, HIGH);   // Traffic yellow light on
      if (currentMillis - previousMillis >= time / 2) {
        digitalWrite(11, LOW);   // Traffic yellow light off
        previousMillis = currentMillis;
        if (set == 0) {
          _currentState = red;  // Transition to red
        } else {
          _currentState = green;  // Transition to green
        }
      }
      break;

    case red:
      digitalWrite(12, HIGH);    // Traffic red light on
      if (currentMillis - previousMillis >= time) {
        digitalWrite(12, LOW);    // Traffic red light off
        previousMillis = currentMillis;
        if (redState == 1) {
          _currentState = yellow; // Transition to yellow
        } else {
          _currentState = pedestrianGreen;  // Transition to pedestrian green
        }
        set = 1;
      }
      break;
  }
}

void pedestrian(unsigned long currentMillis) {
  switch (_targetState) {
    case pedestrianGreen:
      digitalWrite(12, HIGH);   // Traffic red light on
      digitalWrite(2, HIGH);    // Pedestrian green light on
      if (currentMillis - pedestrianMillis >= time / 5) {
        digitalWrite(2, LOW);  // Pedestrian green light off
        pedestrianMillis = currentMillis;
        redState = 1;          // Set redState to 1 after pedestrian light
        _targetState = yellow; // Transition to yellow pedestrian light
      }
      break;

    case yellow:
      digitalWrite(12, HIGH);  // Traffic red light on
      digitalWrite(11, HIGH);  // Traffic yellow light on
      if (currentMillis - pedestrianMillis >= time / 2) {
        digitalWrite(11, LOW); // Traffic yellow light off
        pedestrianMillis = currentMillis;
        _targetState = pedestrianRed;
      }
      break;

    case pedestrianRed:
      digitalWrite(12, LOW);   // Traffic red light off
      digitalWrite(3, HIGH);   // Pedestrian red light on
      if (currentMillis - pedestrianMillis >= time * 2) {
        digitalWrite(3, LOW);  // Pedestrian red light off
        redState = 0;           // Reset redState after pedestrian phase
        _targetState = _currentState; // Return to the traffic state
      }
      break;
  }
}

void buttonPressed() {
  // When the button is pressed, set the pedestrian phase
  if (digitalRead(5) == HIGH) {  // Button pressed (active HIGH)
    redState = 0;  // Trigger pedestrian phase
    _targetState = pedestrianGreen;  // Set to pedestrian green state
    previousMillis = millis();  // Reset the timing
  }
}

void receiveEvent(int event) {
  // Handle Wire communication event and set redState
  redState = Wire.read();
}
