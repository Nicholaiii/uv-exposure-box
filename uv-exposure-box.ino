#include <TM1637Display.h>

/* Display */
const int CLK = 3;
const int DIO = 4;

const int killSwitch = 2; /* Nano has interrupts on 2+3 */
const int toggleButton = 5;
const int incButton = 6;
const int startButton = 7;

const int relayOne = 11;
const int relayTwo = 12;

const int indicator = 8;

int countDownTime = 10; /* initial time set to 10 seconds */
unsigned long startedAt = 0;  /* will store what milli the run started */
unsigned long previousMillis = 0; /* will store last cd update */
volatile bool running = false; /* not running at start */
const long interval = 1000; /* update interval in millis */

const TM1637Display display = TM1637Display(CLK, DIO);

const uint8_t done[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};

void setup () {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(indicator, OUTPUT);
  pinMode(relayOne, OUTPUT);
  pinMode(relayTwo, OUTPUT);

  pinMode(incButton, INPUT_PULLUP);
  pinMode(toggleButton, INPUT_PULLUP);
  pinMode(startButton, INPUT_PULLUP);
  pinMode(killSwitch, INPUT_PULLUP);

  /* Should be maybe lower ? */
  display.setBrightness(5);
  renderDisplay();

  attachInterrupt(digitalPinToInterrupt(killSwitch), killInterrupt, CHANGE);
}

void loop () {
  if (digitalRead(startButton) == LOW
  && digitalRead(killSwitch) == LOW) begin();
  
  if (running) return countdown();

  handleInput();
}

void killInterrupt () {
  /* Killswitch goes HIGH when disconnected from magnet */
  const bool state = digitalRead(killSwitch);
  if (state == HIGH) setState(false);
  digitalWrite(LED_BUILTIN, !state); 
}

void relay (bool state) {
  digitalWrite(relayOne, state);
  digitalWrite(relayTwo, state);
}

void renderDisplay () {
  int minutes = countDownTime / 60;
  int seconds = countDownTime % 60;
  display.showNumberDecEx(minutes * 100 + seconds, 0x80>>1, true);
}

void countdown () {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    /* Store that we just updated */
    previousMillis = currentMillis;
    countDownTime -= 1;    
    if (countDownTime == 0) return finish();
    renderDisplay();
  }  
}

void begin () {
  if (countDownTime == 0 || running) return renderDisplay();
  previousMillis = millis();
  setState(true);
}

void finish () {
  setState(false);
  previousMillis = 0;
  display.clear();
	display.setSegments(done);
}

void setState (bool state) {
  running = state;
  relay(state);
  digitalWrite(indicator, state);
}

void handleInput () {
 if (digitalRead(incButton) == LOW) {
    countDownTime += digitalRead(toggleButton) == HIGH ? 1 : 60;
    renderDisplay();
    delay(200);
  }
}
