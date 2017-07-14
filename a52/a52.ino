#include <SPI.h>
#include <SD.h>
#include <math.h>
#include <stdio.h>
#include <DueTimer.h>

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

int currentBar = 0;
byte rotatingBar[4][9] = {
  {0, 0, 0, 
   1, 1, 1,
   0, 0, 0},
  {0, 0, 1, 
   0, 1, 0,
   1, 0, 0},
  {0, 1, 0, 
   0, 1, 0,
   0, 1, 0},
  {1, 0, 0, 
   0, 1, 0,
   0, 0, 1}
};

DueTimer timer;


//hexcodes auf datenblatt seite 14

// AUFGABE 5.1.1 

void reset() {
  digitalWrite(RST, LOW);
  delay(500);
  digitalWrite(RST, HIGH);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(RST, OUTPUT);
  pinMode(DC, OUTPUT);
  
  SD.begin(4);
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
}

void initializeBuffer() {
  for (int x=0; x<LCD_X; x++) {
    for (int y=0; y<LCD_Y; y++) {
      puffer[x][y] = 0;
    }
  }
}

void writeRegister(byte data) {
  SPI.beginTransaction(SCE, SPISettings(MAX_SPEED, DATA_ORDER, DATA_MODE));
  digitalWrite(DC, LOW);
  SPI.transfer(SCE, data);
  digitalWrite(DC, HIGH);
  SPI.endTransaction();
}

void loop() {
  if (Serial.available() > 0) {
    perform(read_string());
  }
}

char* read_string() {
  char* s = (char*) calloc(100, sizeof(char));
  
  for (int i=0; i<100 && Serial.available() > 0; i++) {
    s[i] = Serial.read();
    delay(20);
  }
  
  return s;
}

void help() {
  Serial.println("Befehle:");
  Serial.println("setContrast(value): Setzt den Bildschirmkontrast, value zwischen 0.0 und 1.0");
  Serial.println("clearDisplay(): loescht den Bildschirminhalt");
  Serial.println("runRotatingBarDemo(): Starte die RotatingBar Demo");
  Serial.println("runStudentIdDemo(): Starte die StudentId Demo");
  Serial.println("stopDemo(): Stoppt die laufende Demo");
}

void setContrast(float value) {
  writeRegister(0x21);
  writeRegister(0x80 | (int) (value * 0x7F));
  writeRegister(0x20);
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

//setzt jeden pixel auf 0, also leerer bildschirm
void clearScreen() {
  initializeBuffer();
  writeBuffer();
}

//AUFGABE 5.1.3
//sollte man das nicht dann mit setPixel tun?
void demo() {
  //aktiviert alle pixel spaltenweise
  for (int x=0; x<LCD_X; x++) {
    for (int y=0; y<LCD_Y; y++) {
      writeRegister(0x80 | x);
      writeRegister(0x40 | y);
      SPI.transfer(SCE, 0xFF);
    }
    delay(20);
  }
  //deaktiviert alle pixel spaltenweise
  for (int x=0; x<LCD_X; x++) {
    for (int y=0; y<LCD_Y; y++) {
      writeRegister(0x80 | x);
      writeRegister(0x40 | y);
      SPI.transfer(SCE, 0);
    }
    delay(20);
  }
}

void demo2() {
  for (int x=0; x<LCD_X; x++) {
    for (int y=0; y<LCD_Y*8; y++) {
      setPixel(x,y,1);
      writeBuffer();
      delay(20);
    }
  }
  for (int x=0; x<LCD_X; x++) {
    for (int y=0; y<LCD_Y*8; y++) {
      setPixel(x,y,0);
      writeBuffer();
      delay(20);
    }
  }
}

void demo3() {
  printString(20,20,"Hallo!");
  printChar(56,20,0x80);
  writeBuffer();
}

void runStudentIdDemo() {
  displayStudent("Teresa Luebeck", "6824033");
  delay(5000);
  clearScreen();
  displayStudent("Tim Kilian", "6824270");
  delay(5000);
  clearScreen();
}

void runRotatingBarDemo() {
  for (int x=0; x<9; x++) {
      if (rotatingBar[currentBar%4][x]) {
        printChar(20+x/3*5,20+x%3*8,0x84 + currentBar % 4);
      } else {
        printChar(20+x/3*5,20+x%3*8,0x20);
      }
  }
  currentBar++;
  writeBuffer();
  
  
}

void stopDemo() {
  timer.stop();
  clearScreen();
}

void displayStudent(char* student, char* matrikelnummer) {
  printString(centerX(strlen(student)*6), centerY(2*8+5), student);
  printString(centerX(strlen(matrikelnummer)*6), centerY(2*8+5)+13, matrikelnummer);
  writeBuffer();
}

int centerX(int content) {
  return (LCD_X-content)/2;
}

int centerY(int content) {
  return (LCD_Y*8-content)/2;
}

// Aufgabe 2
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

void listDirectory(char* directory) {
  File dir = SD.open(directory);
  File file;
  while (file = dir.openNextFile()) {
    Serial.println(file.name());
  }
}

bool doesFileExist(char* file) {
  Serial.print("Check if file '"); Serial.print(file); Serial.println("' exists");
  return SD.exists(file);
}

char* outputFileToSerial(char* filename) {
  Serial.print("Open file '"); Serial.print(filename); Serial.println("'");
  File file = SD.open(filename);
  Serial.print("Read file '"); Serial.print(filename); Serial.println("'");
  
  char c;
  char* output = (char*) calloc(8096, sizeof(char));
  int i = 0;
  
  while (file.available()) {
    // Serial.print(c); Serial.print(" "); Serial.println(output);
    output[i++] = file.read();
  }
  
  Serial.print(output);
  return output;
}

//Macht in oberer Display-Zeile noch müll. WHY? und noch nur für text
void outputFileToLCD(char* file) {
  char* ext = fileExtension(file);
  
  if (ext == ".txt") outputTxtToLCD(file);
  else if (ext[0] == '.' && ext[1] == 'i' && ext[2] == 'm' && ext[3] == 'g') outputImgToLCD(file);
  else { printString(0, 0, "Unbekannte"); printString(0,8,"Dateinendung"); }

  writeBuffer();
}

void outputTxtToLCD(char* file) {
  char* output = outputFileToSerial(file);
  
  for (int y=0; y < 6; y++) {
    char* output_line = (char*) calloc(14, sizeof(char));
    for (int i=0; i<14 && y*14+i < strlen(output); i++) {
      if (output[y*14+i] == '\n') break;
      output_line[i] = output[y*14+i];
    }
    printString(0, y*8, output_line);
  }
  
  if (6*14 < strlen(output)) {
    printString(0, 0, "TXT_SIZE_ERR");
  }
}

void outputImgToLCD(char* file) {
  char* output = outputFileToSerial(file);
  Serial.println("");
  
  Serial.println("outputImgToLCD");

  int x,y;
  char* data = (char*) calloc(8064, sizeof(char));
  
  
  
  if (sscanf(output, "%d,%d", &x, &y) == 2) {
    Serial.print("x: "); Serial.println(x);
    Serial.print("y: "); Serial.println(y);
    
    bool r = false;
    for (int i=0, j=0; i<strlen(output); i++) {
      if (output[i]=='\n') r=true;
      if (r && (output[i]=='1' || output[i]=='0')) {
        data[j++] = output[i];
      } 
    }
    
    for (int yi=centerY(y); yi<centerY(y)+y; yi++) {
      for (int xi=centerX(x); xi<centerX(x)+x; xi++) {
        setPixel(xi,yi,data[(yi-centerY(y))*y+xi-centerX(x)]=='1');
      }
    }
  } else {
    Serial.println("not worked");
  }
}

char* fileExtension(char* file) {
  char* ext = (char*) calloc(4, sizeof(char));
  for (int i=0; i<4; i++) {
    ext[3-i] = file[strlen(file)-1-i];
  }
  Serial.print("Dateiendung: "); Serial.println(ext);
  return ext;
}

void perform(char* command) {
  Serial.print("parse: "); Serial.println(command);
  char c;
  float contrast;
  char dir[] = "";
  char file[] = "";
  delay(100);
  
  if (sscanf(command, "help()")) {
    
    help();
    
  } else if (sscanf(command, "setContrast(%f%c%s", &contrast, &c) == 2 && c==')') {
    
    setContrast(contrast);
    
  } else if (sscanf(command, "clearDisplay()")) {
    
    clearScreen();
    
  } else if (sscanf(command, "runRotatingBarDemo()")) {
    
    if (timer.configure(5, runRotatingBarDemo)) {
      timer.start();
    }
    
  } else if (sscanf(command, "stopDemo()")) {
    
    stopDemo();
    
  } else if (sscanf(command, "runStudentIdDemo()")) {
    
    runStudentIdDemo();
    
  } else if (sscanf(command, "listDirectory(%s)", &dir) == 1) {
    listDirectory(dir);
    
  } else if (sscanf(command, "doesFileExist(%s)", &file) == 1) {
    
    if (doesFileExist(file)) {
      Serial.println("File exists");
    } else {
      Serial.println("File not exists");
    }
    
  } else if (sscanf(command, "outputFileToSerial(%s)", &file) == 1) {
    
    if (doesFileExist(file)) {
      outputFileToSerial(file);
    }
    
  } else if (sscanf(command, "outputFileToLCD(%s)", &file) == 1) {
    
    outputFileToLCD(file);
    
  } else {
    Serial.println("Befehl verweigert: nicht bekannt, versuche help()");
  }
}

