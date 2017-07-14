#include <Servo.h>
#include <math.h>
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

float velocity_x = 0;
float velocity_y = 0;

int button_state = 0;
int laser_state = LOW;

bool playing = false;

DueTimer timer_handle;
DueTimer timer_play;
DueTimer timer_controls;

void toggleButton() { button_state = 0; }

bool pressed(int current) { return current == LOW && button_state == 10; }

bool bothPressed(int btn1, int btn2) { return btn1 == LOW && btn2 == LOW && button_state == 10; }

void setup() {
  Serial.begin(9600);
  
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  pinMode(SEL, INPUT_PULLUP);
  
  pinMode(LASER, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(BTN_1), toggleButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_2), toggleButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(SEL), toggleButton, FALLING);
  
  if (timer_handle.configure(200, handle)) timer_handle.start();
  if (timer_play.configure(2, play)) timer_play.start();
  if (timer_controls.configure(40, moveServoWithVelocityControl)) timer_controls.start();
  
  servo_x.attach(SERVO_X);
  servo_y.attach(SERVO_Y);
}

void loop() {
  digitalWrite(LASER, laser_state);
}

void handle() {
  int current_button_state_1 = digitalRead(BTN_1);
  int current_button_state_2 = digitalRead(BTN_2);
  int current_analog_button_state = digitalRead(SEL);
  button_state++;
  
  if (bothPressed(current_button_state_2, current_analog_button_state)) {
    
  }

  if (pressed(current_button_state_1)) {
    Serial.println("Laser pressed");
    laser_state = !laser_state;
  }
  
  if (pressed(current_button_state_2)) {
    Serial.println("Play record");
    current = head;
    playing = true;
  }
  
  if (pressed(current_analog_button_state)) {
    Serial.println("Save.");
    save();
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
    
    // Serial.print("X: "); Serial.println(angle_x);
    // Serial.print("Y: "); Serial.println(angle_y);
  }
}

void save() {
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

void play() {  
  if (playing && current != NULL) {
    int x = current->x;
    int y = current->y;
    
    servo_x.write(x);
    servo_y.write(y);
    
    Serial.println(x);
    Serial.println(y);
    
    current = current->next;
  }
  
  if (current == NULL) playing = false;
}
