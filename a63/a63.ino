#include <Servo.h>
#include <SPI.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <DueTimer.h>

#define BTN_1 5
#define BTN_2 6
#define SEL 37
#define OUT_Y A9
#define OUT_X A8
#define SERVO_X 11
#define SERVO_Y 12
#define LASER 40

Servo servo_x;
Servo servo_y;

int angle_x = 90;
int angle_y = 90;

typedef struct angle {
  struct angle *next;
  int x;
  int y;
} angle_t;
angle_t *head;
angle_t *current;
int savedStates = 0;

float velocity_x = 0;
float velocity_y = 0;

int button_state = 0;
int laser_state = LOW;

bool playing = false;

DueTimer timer_handle;
DueTimer timer_play;
DueTimer timer_controls;

#define MAX_SPEED  1000000
#define DATA_ORDER MSBFIRST
#define DATA_MODE  SPI_MODE0

#define LCD_X 84
#define LCD_Y 6

#define SCE 8
#define RST 9
#define DC 10

int puffer[LCD_X][LCD_Y];

byte font[][5] = {
 {0x00, 0x00, 0x00, 0x00, 0x00} // 20  
,{0x00, 0x00, 0x5f, 0x00, 0x00} // 21 !
,{0x00, 0x07, 0x00, 0x07, 0x00} // 22 "
,{0x14, 0x7f, 0x14, 0x7f, 0x14} // 23 #
,{0x24, 0x2a, 0x7f, 0x2a, 0x12} // 24 $
,{0x23, 0x13, 0x08, 0x64, 0x62} // 25 %
,{0x36, 0x49, 0x55, 0x22, 0x50} // 26 &
,{0x00, 0x05, 0x03, 0x00, 0x00} // 27 '
,{0x00, 0x1c, 0x22, 0x41, 0x00} // 28 (
,{0x00, 0x41, 0x22, 0x1c, 0x00} // 29 )
,{0x14, 0x08, 0x3e, 0x08, 0x14} // 2a *
,{0x08, 0x08, 0x3e, 0x08, 0x08} // 2b +
,{0x00, 0x50, 0x30, 0x00, 0x00} // 2c ,
,{0x08, 0x08, 0x08, 0x08, 0x08} // 2d -
,{0x00, 0x60, 0x60, 0x00, 0x00} // 2e .
,{0x20, 0x10, 0x08, 0x04, 0x02} // 2f /
,{0x3e, 0x51, 0x49, 0x45, 0x3e} // 30 0
,{0x00, 0x42, 0x7f, 0x40, 0x00} // 31 1
,{0x42, 0x61, 0x51, 0x49, 0x46} // 32 2
,{0x21, 0x41, 0x45, 0x4b, 0x31} // 33 3
,{0x18, 0x14, 0x12, 0x7f, 0x10} // 34 4
,{0x27, 0x45, 0x45, 0x45, 0x39} // 35 5
,{0x3c, 0x4a, 0x49, 0x49, 0x30} // 36 6
,{0x01, 0x71, 0x09, 0x05, 0x03} // 37 7
,{0x36, 0x49, 0x49, 0x49, 0x36} // 38 8
,{0x06, 0x49, 0x49, 0x29, 0x1e} // 39 9
,{0x00, 0x36, 0x36, 0x00, 0x00} // 3a :
,{0x00, 0x56, 0x36, 0x00, 0x00} // 3b ;
,{0x08, 0x14, 0x22, 0x41, 0x00} // 3c <
,{0x14, 0x14, 0x14, 0x14, 0x14} // 3d =
,{0x00, 0x41, 0x22, 0x14, 0x08} // 3e >
,{0x02, 0x01, 0x51, 0x09, 0x06} // 3f ?
,{0x32, 0x49, 0x79, 0x41, 0x3e} // 40 @
,{0x7e, 0x11, 0x11, 0x11, 0x7e} // 41 A
,{0x7f, 0x49, 0x49, 0x49, 0x36} // 42 B
,{0x3e, 0x41, 0x41, 0x41, 0x22} // 43 C
,{0x7f, 0x41, 0x41, 0x22, 0x1c} // 44 D
,{0x7f, 0x49, 0x49, 0x49, 0x41} // 45 E
,{0x7f, 0x09, 0x09, 0x09, 0x01} // 46 F
,{0x3e, 0x41, 0x49, 0x49, 0x7a} // 47 G
,{0x7f, 0x08, 0x08, 0x08, 0x7f} // 48 H
,{0x00, 0x41, 0x7f, 0x41, 0x00} // 49 I
,{0x20, 0x40, 0x41, 0x3f, 0x01} // 4a J
,{0x7f, 0x08, 0x14, 0x22, 0x41} // 4b K
,{0x7f, 0x40, 0x40, 0x40, 0x40} // 4c L
,{0x7f, 0x02, 0x0c, 0x02, 0x7f} // 4d M
,{0x7f, 0x04, 0x08, 0x10, 0x7f} // 4e N
,{0x3e, 0x41, 0x41, 0x41, 0x3e} // 4f O
,{0x7f, 0x09, 0x09, 0x09, 0x06} // 50 P
,{0x3e, 0x41, 0x51, 0x21, 0x5e} // 51 Q
,{0x7f, 0x09, 0x19, 0x29, 0x46} // 52 R
,{0x46, 0x49, 0x49, 0x49, 0x31} // 53 S
,{0x01, 0x01, 0x7f, 0x01, 0x01} // 54 T
,{0x3f, 0x40, 0x40, 0x40, 0x3f} // 55 U
,{0x1f, 0x20, 0x40, 0x20, 0x1f} // 56 V
,{0x3f, 0x40, 0x38, 0x40, 0x3f} // 57 W
,{0x63, 0x14, 0x08, 0x14, 0x63} // 58 X
,{0x07, 0x08, 0x70, 0x08, 0x07} // 59 Y
,{0x61, 0x51, 0x49, 0x45, 0x43} // 5a Z
,{0x00, 0x7f, 0x41, 0x41, 0x00} // 5b [
,{0x02, 0x04, 0x08, 0x10, 0x20} // 5c ¥
,{0x00, 0x41, 0x41, 0x7f, 0x00} // 5d ]
,{0x04, 0x02, 0x01, 0x02, 0x04} // 5e ^
,{0x40, 0x40, 0x40, 0x40, 0x40} // 5f _
,{0x00, 0x01, 0x02, 0x04, 0x00} // 60 `
,{0x20, 0x54, 0x54, 0x54, 0x78} // 61 a
,{0x7f, 0x48, 0x44, 0x44, 0x38} // 62 b
,{0x38, 0x44, 0x44, 0x44, 0x20} // 63 c
,{0x38, 0x44, 0x44, 0x48, 0x7f} // 64 d
,{0x38, 0x54, 0x54, 0x54, 0x18} // 65 e
,{0x08, 0x7e, 0x09, 0x01, 0x02} // 66 f
,{0x0c, 0x52, 0x52, 0x52, 0x3e} // 67 g
,{0x7f, 0x08, 0x04, 0x04, 0x78} // 68 h
,{0x00, 0x44, 0x7d, 0x40, 0x00} // 69 i
,{0x20, 0x40, 0x44, 0x3d, 0x00} // 6a j 
,{0x7f, 0x10, 0x28, 0x44, 0x00} // 6b k
,{0x00, 0x41, 0x7f, 0x40, 0x00} // 6c l
,{0x7c, 0x04, 0x18, 0x04, 0x78} // 6d m
,{0x7c, 0x08, 0x04, 0x04, 0x78} // 6e n
,{0x38, 0x44, 0x44, 0x44, 0x38} // 6f o
,{0x7c, 0x14, 0x14, 0x14, 0x08} // 70 p
,{0x08, 0x14, 0x14, 0x18, 0x7c} // 71 q
,{0x7c, 0x08, 0x04, 0x04, 0x08} // 72 r
,{0x48, 0x54, 0x54, 0x54, 0x20} // 73 s
,{0x04, 0x3f, 0x44, 0x40, 0x20} // 74 t
,{0x3c, 0x40, 0x40, 0x20, 0x7c} // 75 u
,{0x1c, 0x20, 0x40, 0x20, 0x1c} // 76 v
,{0x3c, 0x40, 0x30, 0x40, 0x3c} // 77 w
,{0x44, 0x28, 0x10, 0x28, 0x44} // 78 x
,{0x0c, 0x50, 0x50, 0x50, 0x3c} // 79 y
,{0x44, 0x64, 0x54, 0x4c, 0x44} // 7a z
,{0x00, 0x08, 0x36, 0x41, 0x00} // 7b {
,{0x00, 0x00, 0x7f, 0x00, 0x00} // 7c |
,{0x00, 0x41, 0x36, 0x08, 0x00} // 7d }
,{0x10, 0x08, 0x08, 0x10, 0x08} // 7e ←
,{0x78, 0x46, 0x41, 0x46, 0x78} // 7f →
// Meine ausgedachten Zeichen
,{0x10, 0x26, 0x20, 0x26, 0x10} // 80 :)
,{0x10, 0x16, 0x70, 0x56, 0x70} // 81 :P
,{0x70, 0x56, 0x70, 0x16, 0x10} // 82 :b
,{0x20, 0x26, 0x20, 0x26, 0x20} // 83 :|
,{0x00, 0x00, 0xFF, 0x00, 0x00} // 84 |
,{0x80, 0x60, 0x18, 0x06, 0x01} // 85 /
,{0x08, 0x08, 0x08, 0x08, 0x08} // 86 _
,{0x01, 0x06, 0x18, 0x60, 0x80} // 87 \
};

void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}
 
 // Converts a given integer x to string str[].  d is the number
 // of digits required in output. If d is more than the number
 // of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }
 
    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';
 
    reverse(str, i);
    str[i] = '\0';
    return i;
}
 
// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint)
{
    int ipart = (int)n;
 
    // Extract floating part
    float fpart = n - (float)ipart;
 
    // convert integer part to string
    int i = intToStr(ipart, res, 0);
 
    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot
 
        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);
 
        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

void toggleButton() { button_state = 0; }

bool pressed(int current) { return current == LOW && button_state == 10; }

bool bothPressed(int btn1, int btn2) { return btn1 == LOW && btn2 == LOW && button_state == 10; }

void reset() { digitalWrite(RST, LOW); delay(500); digitalWrite(RST, HIGH); }

void initializeBuffer() { for (int x=0; x<LCD_X; x++) for (int y=0; y<LCD_Y; y++) puffer[x][y] = 0; }

void writeRegister(byte data) {
  SPI.beginTransaction(SCE, SPISettings(MAX_SPEED, DATA_ORDER, DATA_MODE));
  digitalWrite(DC, LOW);
  SPI.transfer(SCE, data);
  digitalWrite(DC, HIGH);
  SPI.endTransaction();
}

void writeBuffer() {
  for (int x=0; x<LCD_X; x++) {
    for (int y=0; y<LCD_Y; y++) {
      writeRegister(0x80 | x);
      writeRegister(0x40 | y);
      SPI.transfer(SCE, puffer[x][y]);
    }
  }
}

bool inRange(int x, int y) {
  return x >= 0 && y >= 0 && x < LCD_X && y < LCD_Y*8;
}

void setPixel(uint8_t x, uint8_t y, bool value) {    
  if (inRange(x, y)) {
    if (value) {
      puffer[x][(y / 8)] |= (int) (pow(2, y % 8));
    } else {
      puffer[x][(y / 8)] &= (0xFF ^ (int) (pow(2, y % 8)));
    }
  }
}

bool printChar(int x, int y, char value) {
  for (int i=0; i<5; i++) {
    if (inRange(x+i,(y/8)+1)) {
      if ((y%8) == 0) puffer[x+i][(y/8)] = 0;
      puffer[x+i][(y/8)] = (puffer[x+i][(y/8)] & (0xFF >> (y%8))) | (font[(int) value-0x20][i] << (y%8));
      if ((y/8)+1 < LCD_Y)
      puffer[x+i][(y/8)+1] = (puffer[x+i][(y/8)+1] & (0xFF << 8-(y%8))) | font[(int) value-0x20][i] >> 8-(y%8);
    } else {
      return false;
    }
  }
  return true;
}

bool printString(int x, int y, char* str) {
  for (int i=0; i<strlen(str); i++) {
    if (!printChar(x+i*6, y, str[i])) {
      Serial.print("Zeichenkette lässt sich nicht darstellen: zu lang.");
      return false;
    }
  }
  return true;
}

void clearScreen() {
  initializeBuffer();
  writeBuffer();
}


int centerX(int content) {
  return (LCD_X-content)/2;
}

int centerY(int content) {
  return (LCD_Y*8-content)/2;
}

void setup() {
  Serial.begin(115200);
  
  pinMode(RST, OUTPUT);
  pinMode(DC, OUTPUT);
  
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(SEL, INPUT_PULLUP);
  
  pinMode(LASER, OUTPUT);
  
  SPI.begin(SCE);
  
  reset();
  
  writeRegister(0x21); // FUNCTION SET
  writeRegister(0x13); // SET BIAS
  writeRegister(0xBB); // SET CONTRAST
  writeRegister(0x04); // SET TEMPERATURE COEFFICIENT
  writeRegister(0x20); // FUNCTION SET
  writeRegister(0x0C); // SET DISPLAY MODE
  writeRegister(0x80);
  writeRegister(0x40);
  
  delay(100);
  
  clearScreen();
  
  attachInterrupt(digitalPinToInterrupt(BTN_1), toggleButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_2), toggleButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(SEL), toggleButton, FALLING);
  
  if (timer_handle.configure(200, handle)) timer_handle.start();
  if (timer_play.configure(20, play)) timer_play.start();
  if (timer_controls.configure(20, moveServoWithVelocityControl)) timer_controls.start();
  
  servo_x.attach(SERVO_X);
  servo_y.attach(SERVO_Y);
}

void loop() {
  digitalWrite(LASER, laser_state);
  
  if (playing) {
    printString(0,0, "          ");
    printString(0,0, "Wiedergabe");
  } else {
    printString(0,0, "           ");
    printString(0,0, "Aufnahme");
  }
  
  printString(0,12, "           ");
  printString(0,12, "X: ");
  char ax[8];
  ftoa(angle_x,ax,4);
  printString(15,12, ax);
  printString(0,24, "           ");
  printString(0,24, "Y: ");
  char ay[8];
  ftoa(angle_y,ay,2);
  printString(15,24, ay);
  char saved[8]; 
  itoa(savedStates,saved,10);
  printString(0,36, "           ");
  printString(0,36, saved);
  printString(15,36, "gesp.");
}

int writeBufferCount = 0;
float temp_x = 0;
float temp_y = 0;

void handle() {
  int current_button_state_1 = digitalRead(BTN_1);
  int current_button_state_2 = digitalRead(BTN_2);
  int current_analog_button_state = digitalRead(SEL);
  button_state++;

  if (pressed(current_button_state_1)) {
    Serial.println("Laser pressed");
    laser_state = !laser_state;
  }
  
  if (pressed(current_button_state_2)) {
    Serial.println("Play record");
    current = head;
    playing = true;
    temp_x = angle_x;
    temp_y = angle_y;
  }
  
  if (pressed(current_analog_button_state)) {
    Serial.println("Save.");
    save();
  }
  
  if (bothPressed(current_button_state_2, current_analog_button_state)) {
    deleteRec();
  }
  
  if (writeBufferCount++%200==0) {
    writeBuffer();
  }
}

// Aufgabe 6.1
void printXY() {
  // X: 0 511 1023
  // Y: 0 511 1023
  
  int out_x = analogRead(OUT_X);
  int out_y = analogRead(OUT_Y);
  
  Serial.print("X: "); Serial.println(out_x);
  Serial.print("X in V: "); Serial.print(out_x / 1023.0 * 3.3); Serial.println(' V');
  Serial.print("Y: "); Serial.println(out_y);
  Serial.print("Y in V: "); Serial.print(out_y / 1023.0 * 3.3); Serial.println(' V');
}

// Aufgabe 6.2
void moveServoWithAnalogStick() {
  servo_x.write((int) analogRead(OUT_X)/5.5);
  servo_y.write((int) analogRead(OUT_Y)/5.5);
}

// Aufgabe 6.3
void moveServoWithVelocityControl() {
  if (!playing) {
    if (angle_x+velocity_x <= 180 && angle_x+velocity_x >= 0) angle_x += (int) velocity_x;
    if (angle_y+velocity_y <= 180 && angle_y+velocity_y >= 0) angle_y += (int) velocity_y;
    
    servo_x.write(angle_x);
    servo_y.write(angle_y);
    
    velocity_x = analogRead(OUT_X)/100.0;
    velocity_y = analogRead(OUT_Y)/100.0;
    velocity_x -= 5;
    velocity_y -= 5;
  }
}

void save() {
  savedStates++;
  if (head == NULL) {
    head = (angle_t*) malloc(sizeof(angle_t));
    head->x = angle_x;
    head->y = angle_y;
    head->next = NULL;
  } else {
    current = head;
    while (current->next != NULL) { 
      current = current->next;
    }
    current->next = (angle_t*) malloc(sizeof(angle_t));
    current->next->x = angle_x;
    current->next->y = angle_y;
    current->next->next = NULL;
  }
}

int nextStep = 0;

void play() {  
  if (playing && current != NULL) {
    int x = current->x;
    int y = current->y;
    
    temp_x += (x - angle_x)/10.0;
    temp_y += (y - angle_y)/10.0;
    servo_x.write((int) temp_x);
    servo_y.write((int) temp_y);
    
    Serial.print(nextStep); Serial.print(" : "); Serial.print(x); Serial.print(" : "); Serial.print(angle_x); Serial.print(" : "); Serial.println(temp_x);
    Serial.print(nextStep); Serial.print(" : "); Serial.print(y); Serial.print(" : "); Serial.print(angle_y); Serial.print(" : "); Serial.println(temp_y);
    
    if (nextStep++ > 8) {
      current = current->next;
      nextStep = 0;
      angle_x = (int) temp_x;
      angle_y = (int) temp_y;
    }
  }
  
  if (current == NULL) playing = false;
}

void deleteRec() {
  head = NULL;
  savedStates = 0;
}
