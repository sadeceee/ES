#include <DueTimer.h>

int pin_red = 7, pin_green = 6, pin_blue = 5;
int pin_red_state = 0, pin_green_state = 0, pin_blue_state = 0;
int button_pin1 = 22, button_pin2 = 23;
int button_pin_state1 = 0, button_pin_state2 = 0;
bool option = true; // 0: "direction", 1: "power"
int button_state = 0;

int pin_motor_standby = 8;
int pin_motor_in1 = 31, pin_motor_in2 = 33;
int power = 0;
int motor_state_1 = LOW, motor_state_2 = HIGH;

DueTimer timer;

void setup() {
  pinMode(pin_red, OUTPUT);
  pinMode(pin_green, OUTPUT);
  pinMode(pin_blue, OUTPUT);
  
  pinMode(pin_motor_standby, OUTPUT);
  pinMode(pin_motor_in1, OUTPUT);
  pinMode(pin_motor_in2, OUTPUT);
  
  pinMode(button_pin1, INPUT_PULLUP);
  pinMode(button_pin2, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(button_pin1), toggleButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(button_pin2), toggleButton, FALLING);
  
  if (timer.configure(200, handle)) timer.start();
}

void loop() {
  analogWrite(pin_red, pin_red_state);
  analogWrite(pin_green, pin_green_state);
  analogWrite(pin_blue, pin_blue_state);

  analogWrite(pin_motor_standby, power);
  digitalWrite(pin_motor_in1, motor_state_1);
  digitalWrite(pin_motor_in2, motor_state_2);
}

void handle() {
  int current_button_state1 = digitalRead(button_pin1);
  int current_button_state2 = digitalRead(button_pin2);
  button_state++;
  
  if (bothPressed(current_button_state1, current_button_state2)) {
    changeOption();
  }
  
  if (option) {
    green();
    if (pressed(current_button_state1)) {
      powerUp();
    }
    else if (pressed(current_button_state2)) {
      powerDown();
    }
  } else {
    red();
    if (pressed(current_button_state1)) {
      turnLeft();
    }
    else if (pressed(current_button_state2)) {
      turnRight();
    }
  }  
}

bool pressed(int current) {
  return current == LOW && button_state == 10;
}

bool bothPressed(int btn1, int btn2) {
  return btn1 == LOW && btn2 == LOW && button_state == 10;
}

void changeOption() {
  option = !option;
}

//green
void powerUp() {
  if (power >= 255) {
    power = 255; 
  } else {
    power += 51;  
  }
}

void powerDown() {
  if (power <= 0) {
    power = 0; 
  } else {
    power -= 51;  
  }
}

//red
void turnLeft() { //cw -> ccw
  motor_state_1 = LOW;
  motor_state_2 = HIGH;
}

void turnRight() { //ccw -> cw
  motor_state_1 = HIGH;
  motor_state_2 = LOW;
}

void green() {
  pin_red_state = 255;
  pin_green_state = 0;
  pin_blue_state = 255;
}

void red() {
  pin_red_state = 0;
  pin_green_state = 255;
  pin_blue_state = 255;
}

void toggleButton() {
   button_state = 0;
}
