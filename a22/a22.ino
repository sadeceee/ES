#include <DueTimer.h>

int pin_state = 0;
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
  attachInterrupt(digitalPinToInterrupt(button_pin2), toggleButton2, FALLING);
  if (timer.configure(200, handleLed)) {
    timer.start();
  }
}

void loop() {
    analogWrite(led_pin, pin_state);
}

void handleLed() {
  int current_button_state1 = digitalRead(button_pin1);
  int current_button_state2 = digitalRead(button_pin2);
  
  if (current_button_state1 == LOW && button_pin_state1++ == 10) {
    increment();
  }
  if (current_button_state2 == LOW && button_pin_state2++ == 10) {
    decrement();
  }
  
}

void increment() {
   if (pin_state >= 255) {
      pin_state = 0;    
    } else {
      pin_state += 51;
    }
}

void decrement() {
   if (pin_state <= 0) {
      pin_state = 255;    
    } else {
      pin_state -= 51;
    }
}

void toggleButton1() {
   button_pin_state1 = 0;
}

void toggleButton2() {
   button_pin_state2 = 0;
}
