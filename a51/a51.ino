#include <SPI.h>

int sce_pin = 8;
int max_speed = 1000;
int data_order;
int data_mode = MODE_0;

void setup() {
  setupDisplay();
}

void setupDisplay() {
  reset();
  SPI.beginTransaction(sce_pin, SPISettings(max_speed, data_order, data_mode));
}

void reset() {
  
}

void loop() {
  
}

void setPixel(int x, int y, int value) {
  if (inRange(x, y)) {
    
  }
  SPI.transfer(sce_pin, data);
}

void demo() {
  
}

bool inRange(int x, int y) {
  return x >= 0 && y >= 0 && x <= 48 && y <= 48;
}
