// Traffic light: RED → GREEN → YELLOW (non-blocking, switch–state pattern)

// --- States as numeric constants (avoid enum in function params) ---
const uint8_t STATE_RED    = 0;
const uint8_t STATE_GREEN  = 1;
const uint8_t STATE_YELLOW = 2;

// --- Pins ---
const int PIN_RED    = 8;
const int PIN_YELLOW = 9;
const int PIN_GREEN  = 10;

// --- Durations (ms) ---
const unsigned long T_RED_MS    = 20000; // 20 s
const unsigned long T_GREEN_MS  = 20000; // 20 s
const unsigned long T_YELLOW_MS = 3000;  // 3 s

// // Faster sim (optional):
// const unsigned long T_RED_MS    = 3000;
// const unsigned long T_GREEN_MS  = 3000;
// const unsigned long T_YELLOW_MS = 800;

uint8_t state = STATE_RED;
unsigned long stateStartMs = 0;

void setLights(bool r, bool y, bool g) {
  digitalWrite(PIN_RED,    r ? HIGH : LOW);
  digitalWrite(PIN_YELLOW, y ? HIGH : LOW);
  digitalWrite(PIN_GREEN,  g ? HIGH : LOW);
}

// No-arg helper: duration of the *current* state
unsigned long durationOf() {
  if (state == STATE_RED)    return T_RED_MS;
  if (state == STATE_GREEN)  return T_GREEN_MS;
  /* state == STATE_YELLOW */ return T_YELLOW_MS;
}

// Use uint8_t instead of a custom enum type to dodge auto-prototype issues
void enterState(uint8_t s) {
  state = s;
  stateStartMs = millis();
  if      (s == STATE_RED)    setLights(true,  false, false);
  else if (s == STATE_GREEN)  setLights(false, false, true);
  else /*  s == STATE_YELLOW*/setLights(false, true,  false);
}

void tryTransition() {
  if (millis() - stateStartMs < durationOf()) return; // time event t20/t3

  switch (state) {
    case STATE_RED:    enterState(STATE_GREEN);  break; // after(20s)
    case STATE_GREEN:  enterState(STATE_YELLOW); break; // after(20s)
    case STATE_YELLOW: enterState(STATE_RED);    break; // after(3s)
  }
}

void setup() {
  pinMode(PIN_RED,    OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN,  OUTPUT);
  enterState(STATE_RED); // deterministic safe start
}

void loop() {
  tryTransition();
}
