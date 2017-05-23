#include <DueTimer.h>
#include <stdio.h>

int pin_red = 10, pin_green = 9, pin_blue = 8;
float red = 0.0, green = 1.0, blue = 1.0;

void setup() {
  pinMode(pin_red, OUTPUT);
  pinMode(pin_green, OUTPUT);
  pinMode(pin_blue, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0 && parse(read_string())) {
    analogWrite(pin_red, intensity(red));
    analogWrite(pin_green, intensity(green));
    analogWrite(pin_blue, intensity(blue));
  }
}

int intensity(float color) {
  return (int) (255*color);
}

char* read_string() {
  char* s = (char*) calloc(100, sizeof(char));
  
  for (int i=0; i<100 && Serial.available() > 0; i++) {
    s[i] = Serial.read();
    delay(20);
  }
  
  return s;
}

bool parse(char* input) {
  Serial.print("parse: "); Serial.println(input);
  
  if (formCorrect(input)) {
    Serial.println("Success");
    return true;
  }
  
  Serial.println("Use format 'setRGB(RED_value, GREEN_value, BLUE_value)'");
  return false;
}

bool formCorrect(char* input) {
  char c;
  bool b = sscanf(input, "setRGB(%f,%f,%f%c%s", &red, &green, &blue, &c) == 4 && c==')'
      && correctInterval(red) && correctInterval(green) && correctInterval(blue);
  return b;
}

bool correctInterval(float color) {
  if (color >= 0 && color <= 1) {
    return true;
  }
  Serial.print(color); Serial.println(" not in interval [0,1]");
  return false;
}
