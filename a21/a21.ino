#include <DueTimer.h>

int pin_state = LOW;
int led_pin = 7;
int button_pin1 = 5;
int button_pin_state1 = 0;

int button_pin2 = 4;
int button_pin_state2;

DueTimer timer;

void setup() {
  pinMode(led_pin, OUTPUT);
  pinMode(button_pin1, INPUT_PULLUP);
  button_pin_state1 = digitalRead(button_pin1);
  attachInterrupt(digitalPinToInterrupt(button_pin1), toggleButton1, FALLING);
  if (timer.configure(200, handleLed)) {
    timer.start();
  }
}

void loop() {
    digitalWrite(led_pin, pin_state);
}

void handleLed() {
  int current_button_state = digitalRead(button_pin1);
  
  if (current_button_state == LOW && button_pin_state1++ == 10) {
    pin_state = !pin_state;
  }
}

void toggleButton1() {
   button_pin_state1 = 0;
}
