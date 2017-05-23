#include <SPI.h>

#define MAX_SPEED  1000000;
#define DATA_ORDER MSBFIRST;
#define DATA_MODE  SPI_MODE0;

enum {SCE_PIN=8, RST_PIN=7};

void setup() {
  pinMode(SCE_PIN, OUTPUT);
  pinMode(RST_PIN, OUTPUT);
  
  reset();
  SPI.beginTransaction(SCE_PIN, SPISettings(MAX_SPEED, DATA_ORDER, DATA_MODE));
}

void reset() {
  digitalWrite(RST_PIN, LOW);
  delay(500);
  digitalWrite(RST_PIN, HIGH);
}

void terminate() {
  SPI.endTransaction();
}

void loop() {
  
}

void setPixel(int x, int y, int value) {
  if (inRange(x, y)) {
    
  }
  SPI.transfer(SCE_PIN, data);
}

void demo() {
  
}

bool inRange(int x, int y) {
  return x >= 0 && y >= 0 && x <= 48 && y <= 48;
}
