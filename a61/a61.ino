#include <Servo.h>
#include <math.h>
// #include <DueTimer.h> // #define USING_SERVO_LIB

#define BTN_1 5
#define BTN_2 6
#define SEL 37
#define OUT_Y A9
#define OUT_X A8
#define SERVO_X 11
#define SERVO_Y 12

Servo servo_x;
Servo servo_y;

int angle_x = 90;
int angle_y = 90;

float velocity_x = 0;
float velocity_y = 0;

void setup() {
  Serial.begin(9600);
  
  servo_x.attach(SERVO_X);
  servo_y.attach(SERVO_Y);
}

void loop() {
  if (angle_x+velocity_x <= 180 && angle_x+velocity_x >= 0) angle_x += (int) velocity_x;
  if (angle_y+velocity_y <= 180 && angle_y+velocity_y >= 0) angle_y += (int) velocity_y;
  
  servo_x.write(angle_x);
  servo_y.write(angle_y);
  
  velocity_x = analogRead(OUT_X)/100.0;
  velocity_y = analogRead(OUT_Y)/100.0;
  velocity_x -= 5;
  velocity_y -= 5;
  Serial.println(velocity_x);
  Serial.println(velocity_y);
}

// Aufgabe 6.1
void printXY() {
  // X: 0 511 1023
  // Y: 0 511 1023
  
  int out_x = analogRead(OUT_X);
  int out_y = analogRead(OUT_Y);
  
  Serial.print("X: "); Serial.println(out_x);
  Serial.print("Y: "); Serial.println(out_y);
}

// Aufgabe 6.2
void moveServoWithAnalogStick() {
  servo_x.write((int) analogRead(OUT_X)/5.5);
  servo_y.write((int) analogRead(OUT_Y)/5.5);
}
