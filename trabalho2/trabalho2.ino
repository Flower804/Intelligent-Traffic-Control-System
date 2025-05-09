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
int Red_LED = 13;
int Yellow_LED = 12;
int Green_LED = 11;

int button = 2;

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

//----------------------FUNCS----------------------
int light_mode = 0;
bool change = false;
unsigned long last_change_time = 0;

//TODO: delete this, they only serve for debugging purposes
unsigned long time_passed;

void lighting_sequence(){
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

  }
}

void count_clicks(){
  light_mode = 1;
  last_change_time = millis();
}

void setup() {
  pinMode(Red_LED ,OUTPUT);
  pinMode(Yellow_LED ,OUTPUT);
  pinMode(Green_LED ,OUTPUT);

  //button
  pinMode(button, INPUT);
  attachInterrupt(0, count_clicks, FALLING);

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

  Serial.begin(9600);
}

unsigned long last_blink;
bool blink = false;

void economy_mode(){
  digitalWrite( Red_LED, LOW);
  digitalWrite( Green_LED, LOW);
  if((millis() - last_blink) >= 5000){
    blink = !blink;
  }
  digitalWrite(Yellow_LED, blink);
}

int pulse;
int distance;
int car_counter = 0;
unsigned long last_car_detected;

void loop() {
  digitalWrite(trig_pin, HIGH);
  delay(2);
  digitalWrite(trig_pin, LOW);
  
  pulse = pulseIn(echo_pin, HIGH);
  distance = (pulse * 0.034)/2;
  if(distance >= 6){
    last_car_detected = millis();
    car_counter++;
  }
  if(car_counter == 10){
    car_counter = 0;
  }
  if((millis() - last_car_detected) >= 20000){
    economy_mode();
  } else {
    lighting_sequence();
  }
  for(int i = 0; i < 8; i++){
    digitalWrite(display_pins[i], display_mode[car_counter][i]);
  }
  Serial.print("distance ");
  Serial.print(distance);
  Serial.print("\n");
  Serial.print("time ");
  Serial.print(millis() - last_car_detected);
  Serial.print("\n");
}
