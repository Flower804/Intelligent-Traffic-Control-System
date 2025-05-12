/* ************************************************************************** */
/*                                                                            */
/*                                                                            */
/*   Trabalho2.ino                                                _____      */
/*                                                               _|___ /      */  
/*   By: Flower :3 <gabrielmoita34@gmail.com                    (_) |_ \      */  
/*   GitHub: Flower804                                           _ ___) |     */  
/*   Created: 2025/05/07 21:58:57 by Flower:3                   (_)____/      */                  
/*                                                                            */
/*                                                                            */
/* ************************************************************************** */

//LEDS
int Red_LED = A1;
int Yellow_LED = A2;
int Green_LED = A3;

int lighting_LED = 11;

//Beeper
int beeper = 13;

//buttons 
int button = 2;

int emergency_button = 5;

//Ultrazonic sensor
int trig_pin = 3;
int echo_pin = 4;

//7-segment display stuff

int top = 10;
int top_right = 6;
int top_left = 7;
int bottom = 9;
int middle = 8;
int bottom_left = 0;
int bottom_right = 1;
int dott = 5;

//inteligente lighting system

int lum_sensor = A0;

const int display_pins[8] = {top, bottom, middle, top_left, bottom_left, bottom_right, top_right};

bool display_mode[10][7] = {
  //top,bottom,middle,top left,bottom left,ponto,bottom right,top right
  {1, 1, 0, 1, 1, 1, 1},
  {0, 0, 0, 0, 0, 1, 1},
  {1, 1, 1, 0, 1, 0, 1},
  {1, 1, 1, 0, 0, 1, 1},
  {0, 0, 1, 1, 0, 1, 1},
  {1, 1, 1, 1, 0, 1, 0},
  {1, 1, 1, 1, 1, 1, 0},
  {1, 0, 0, 0, 0, 1, 1},
  {1, 1, 1, 1, 1, 1, 1},
  {1, 0, 1, 1, 0, 1, 1}
};

bool other_modes[2][7] = {
  {1, 0, 1, 1, 1, 0, 1},
  {1, 1, 1, 1, 1, 0, 0}
};

//----------------------FUNCS----------------------
int light_mode = 0;
bool change = false;
unsigned long last_change_time = 0;
bool beeper_to_play = false;
unsigned long last_beep = 0;

//TODO: delete this, they only serve for debugging purposes
unsigned long time_passed;

void lighting_sequence(){
  if(beeper_to_play){
    if(millis() - last_beep >= 2000){
      digitalWrite(beeper, HIGH);
    } else {
      digitalWrite(beeper, LOW);
    }
    for(int i = 0; i < 8; i++){
      digitalWrite(display_pins[i], other_modes[0][i]);
    }
  }

  if((light_mode == 0) && ((millis() - last_change_time) >= 9000)){
    light_mode = 1;

    last_change_time = millis();
  } else if((light_mode == 1) && ((millis() - last_change_time) >= 3000)){
    light_mode = 2;

    last_change_time = millis();
  } else if((light_mode == 2) && ((millis() - last_change_time) >= 5000)){
    light_mode = 0;

    last_change_time = millis();
  }

  if(light_mode == 0){//turn on green LED
    digitalWrite(Green_LED, HIGH);
    digitalWrite(Yellow_LED, LOW);
    digitalWrite(Red_LED, LOW);

  } else if(light_mode == 1){//turn on yellow LED
    digitalWrite(Yellow_LED, HIGH);
    digitalWrite(Red_LED, LOW);
    digitalWrite(Green_LED, LOW);

  } else if(light_mode == 2){ //assume mode = 2 | turn on RED LED
    digitalWrite(Red_LED, HIGH);
    digitalWrite(Green_LED, LOW);
    digitalWrite(Yellow_LED, LOW);

    beeper_to_play = false;
  }
}

//system interrupt
void pedestrian_request(){
  light_mode = 1;
  beeper_to_play = true;
  last_change_time = millis();
}

bool emergency = false;
void emergency_request(){
  emergency = !emergency;
}

void setup() {
  //lights 
  pinMode(Red_LED ,OUTPUT);
  pinMode(Yellow_LED ,OUTPUT);
  pinMode(Green_LED ,OUTPUT);
  pinMode(light_mode, OUTPUT);

  //button
  pinMode(button, INPUT);
  attachInterrupt(0, pedestrian_request, FALLING);

  pinMode(emergency_button, INPUT);
  attachInterrupt(0, emergency_request, FALLING);

  //distance sensors
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);

  //segment display pinmodes
  pinMode(top ,OUTPUT);
  pinMode(top_right ,OUTPUT);
  pinMode(top_left ,OUTPUT);
  pinMode(bottom ,OUTPUT);
  pinMode(middle ,OUTPUT);
  pinMode(bottom_left ,OUTPUT);
  pinMode(bottom_right ,OUTPUT);
  pinMode(dott ,OUTPUT);

  //other stuff
  pinMode(beeper, OUTPUT);

  //lumi sense stuff
  pinMode(lum_sensor, INPUT);

  Serial.begin(9600);
}

unsigned long last_blink;
bool blink = false;
bool car = false;

void economy_mode(){
  digitalWrite( Red_LED, LOW);
  digitalWrite( Green_LED, LOW);
  if((millis() - last_blink) >= 5000){
    blink = !blink;
  }
  digitalWrite(Yellow_LED, blink);
}

int current_lumi;
int intensity; //will vary beetwheen 0 -> 255;
//map(frequency, 0, 1023, MIN_VALUE, MAX_VALUE)

void verify_light(){
  current_lumi = analogRead(lum_sensor);
  Serial.println(current_lumi);
  if(car){
    for(intensity; intensity < 255; intensity = intensity + 10){
      analogWrite(lighting_LED, intensity);
    }
    car = false;
  }
}

bool increasing = false;

void emergency_mode(){
  if(millis() - last_beep >= 2000){
    digitalWrite(beeper, HIGH);
  } else {
    digitalWrite(beeper, LOW);
  }
  //===== pisca rapidamente itensidade variavel ======
  if(increasing){
    for(int i = 0; i < 255; i++){
      analogWrite(lighting_LED, i);
    }
  } else {
    for(int i = 255; i < 0; i--){
      analogWrite(lighting_LED, i);
    }
  }
  //==================================================
  for(int i = 0; i < 8; i++){
    digitalWrite(display_pins[i], display_mode[2][i]);
  }
}

int pulse;
int distance;
int car_counter = 0;
unsigned long last_car_detected;
int lumi = 0;
int mode = 0;

void loop() {
  digitalWrite(trig_pin, HIGH);
  delay(2);
  digitalWrite(trig_pin, LOW);
  
  pulse = pulseIn(echo_pin, HIGH);
  distance = (pulse * 0.034)/2;

  if(distance >= 6){
    last_car_detected = millis();
    car = true;
    car_counter++;
  }
  if(car_counter == 10){
    car_counter = 0;
  }
  //decide modes
  if((millis() - last_car_detected) >= 20000){
    mode = 1;
  } else {
    mode = 0;
  }
  //this needs to be here because of the defenition of mode 
  if(emergency){
    mode = 3;
  }
  switch(mode){
    case(0):
      lighting_sequence();
    case(1):
      economy_mode();
    case(2): //emergency case
      emergency_mode();
  } 
  for(int i = 0; i < 8; i++){
    digitalWrite(display_pins[i], display_mode[car_counter][i]);
  }
  verify_light();
  //Serial.print("distance ");
  //Serial.print(distance);
  //Serial.print("\n");
  //Serial.print("time ");
  //Serial.print(millis() - last_car_detected);
  //Serial.print("\n");
  //Serial.print("lumi ");
  //Serial.print(lumi);
  //Serial.print("\n");
}
