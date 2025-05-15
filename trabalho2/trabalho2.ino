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
int Red_LED = 6;
int Yellow_LED = 7;
int Green_LED = 9;

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

int bit_1 = 9;
int bit_2 = 10;
int bit_3 = 12;
int bit_4 = 13;

int bit_pins[4] = {bit_1, bit_2, bit_3, bit_4};

//inteligente lighting system

int lum_sensor = A0;

//----------------------FUNCS----------------------

void dectobin(int n){
  int binaryNumber[4];

  int i = 0;
  while(n > 0){
    binaryNumber[i] = n % 2;
    n = n / 2;
    i++;
  }

  for(int j = i - 1; j >= 0; j--){
    digitalWrite(bit_pins[j], binaryNumber[j]);
  }
}

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
    }
  } else {
    digitalWrite(beeper, LOW);
  }
  if((light_mode == 0) && ((millis() - last_change_time) >= 9000)){
    light_mode = 1;

    last_change_time = millis();
    beeper_to_play = false;
  } else if((light_mode == 1) && ((millis() - last_change_time) >= 3000)){
    light_mode = 2;

    last_change_time = millis();
  } else if((light_mode == 2) && ((millis() - last_change_time) >= 5000)){
    light_mode = 0;

    last_change_time = millis();
  }

  if(light_mode == 0){//turn on green LED
    beeper_to_play = false;

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

//system interrupt
void pedestrian_request(){
  light_mode = 1;
  beeper_to_play = true;
  last_change_time = millis();
  lighting_sequence();
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

  pinMode(emergency_button, INPUT_PULLUP); //dont forget to point out that this is important on the essay

  //distance sensors
  pinMode(echo_pin, INPUT);
  pinMode(trig_pin, OUTPUT);

  //7segment display binary outputs
  pinMode(bit_1, OUTPUT);
  pinMode(bit_2 , OUTPUT);
  pinMode(bit_3 , OUTPUT);
  pinMode(bit_4 , OUTPUT);    

  //other stuff
  pinMode(beeper, OUTPUT);

  //lumi sense stuff
  pinMode(lum_sensor, INPUT);

  Serial.begin(9600);
}

unsigned long last_blink;
bool blink = false;
bool car = false;
int pulse;
int distance;
int car_counter = 0;
unsigned long last_car_detected;
int last_distance;

void economy_mode(int car_counter){
  digitalWrite( Red_LED, LOW);
  digitalWrite( Green_LED, LOW);
  if((millis() - last_blink) >= 5000){
    blink = !blink;
  }
  digitalWrite(Yellow_LED, blink);

  dectobin(car_counter);
}

int current_lumi;
int intensity; //will vary beetwheen 0 -> 255;

void verify_light(){
  current_lumi = analogRead(lum_sensor);
  if(car){
    for(intensity; intensity < 255; intensity = intensity + 10){
      analogWrite(lighting_LED, intensity);
      delay(50);
    }
    car = false;
  }
  if((!car) && (current_lumi > 1000)){
    for(intensity; intensity <= 127; intensity = intensity - 10){
      analogWrite(lighting_LED, 127);
      delay(50);
    }
  }
}

bool increasing = false;

void emergency_mode(){
  digitalWrite( Red_LED, LOW);  
  digitalWrite( Yellow_LED, LOW);
  if(millis() - last_beep >= 2000){
    digitalWrite(beeper, HIGH);
  } else {
    digitalWrite(beeper, LOW);
  }
  //===== pisca rapidamente itensidade variavel ======
  if(increasing){
    for(int i = 0; i < 255; i = i + 10){
      analogWrite(Green_LED, i);
      delay(10);
    }
  } else {
    digitalWrite(Green_LED, LOW);
  }
  increasing = !increasing;
}

int lumi = 0;
int mode;
int buttonState;
bool emergency = false;

void loop() {
  buttonState = digitalRead(emergency_button);

  if(!buttonState){
    Serial.println("BUTTON CLICKED");
    emergency = !emergency;
  }

  //decide modes
  if((millis() - last_car_detected) >= 20000){
    mode = 1;
  } else {
    mode = 0;
  }

  digitalWrite(trig_pin, HIGH);
  delay(2);
  digitalWrite(trig_pin, LOW);

  pulse = pulseIn(echo_pin, HIGH);
  distance = (pulse * 0.034)/2;

  if((distance <= 6) && (distance != last_distance)){
    last_car_detected = millis();
    car = true;
    car_counter++;
    last_distance = distance;
  }
  if(car_counter == 10){
    car_counter = 0;
  }

  //this needs to be here because of the defenition of mode 
  if(emergency){
    mode = 2;
  }
  if(!emergency){
    digitalWrite(beeper, LOW);
  }
  switch(mode){
    case(0):
      lighting_sequence();
      Serial.println("lighting_sequence");
      break;
    case(1):
      economy_mode(car_counter);
      Serial.println("economy_mode");
      break;
    case(2): //emergency case
      emergency_mode();
      Serial.println("emergency_mode");
      break;
  } 
  verify_light();
  printdata();

  delay(1000); //TODO: eliminate this
}

void printdata(){
  Serial.println("======================");
  Serial.print("distance: ");
  Serial.print(distance);
  Serial.print("\ntime: ");
  Serial.print(millis() - last_car_detected);
  Serial.print("\nlumi: ");
  Serial.print(current_lumi);
  Serial.print("\ncar count: ");
  Serial.print(car_counter);
  Serial.print("\n");
  Serial.println("======================");
}