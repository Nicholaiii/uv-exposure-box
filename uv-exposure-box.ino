#include <TM1637Display.h>

// Define the connections pins
const int CLK = 3;
const int DIO = 4;

const int incButton = 5;
const int toggleButton = 6;
const int startButton = 7;
const int killSwitch = 2; /* Nano has interrupts on 2+3 */
const int relayOne = 12;
const int relayTwo = 13;

bool running = false; /* not running at start */
int countDownTime = 1; /* initial time set to 1 second */

const TM1637Display display = TM1637Display(CLK, DIO);

const uint8_t done[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
};

void setup () {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(relayOne, OUTPUT);
  pinMode(relayTwo, OUTPUT);

  pinMode(incButton, INPUT_PULLUP);
  pinMode(toggleButton, INPUT_PULLUP);
  pinMode(startButton, INPUT_PULLUP);
  pinMode(killSwitch, INPUT_PULLUP);
  /* Should be maybe lower ? */
  display.setBrightness(5);
  renderDisplay();
}

void relay (bool state) {
  digitalWrite(relayOne, state);
  digitalWrite(relayTwo, state);
}

bool handleDelay () {
  for (int i = 20; i >=1; i--) {
    if (!running) return false;
    if (digitalRead(killSwitch) == LOW) {
      delay(50);
      return true;
    } else {
      running = false;
      relay(false);
      return false;
    }
  }
}

void renderDisplay () {
  int minutes = countDownTime / 60;
  int seconds = countDownTime % 60;
  display.showNumberDecEx(minutes * 100 + seconds, 0x80>>1, true);
}

void countdown () {
  relay(running);
  if (!handleDelay()) return;
  countDownTime -= 1;
  if (countDownTime == 0) {
    return finish();
  }
  renderDisplay();
}

void finish () {
  stop();
  display.clear();
	display.setSegments(done);
}

void stop () {
  running = false;
  relay(false);
  digitalWrite(LED_BUILTIN, 0);
}

void loop () {
  digitalWrite(LED_BUILTIN, digitalRead(killSwitch));

  if (digitalRead(startButton) == LOW) {
    if (countDownTime == 0) return renderDisplay();
    running = true;  
  }
  
  if (running) {
    return countdown();
  }

  if (digitalRead(incButton) == LOW) {
    if (digitalRead(toggleButton) == LOW) {
      countDownTime += 60; // increase time by 1 minute
    } else {
      countDownTime += 1; // increase time by 1 seconds
    }
    renderDisplay();
    delay(200);
  }
}
