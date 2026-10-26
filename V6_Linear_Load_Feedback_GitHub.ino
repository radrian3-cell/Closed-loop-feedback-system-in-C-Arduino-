#include "HX711.h"

const int DT_PIN = 3;
const int SCK_PIN = 2;
const int RPWM = 9;
const int LPWM = 8;
const float MAX_FORCE = 0.98;
float MEASURED_FORCE = 0.0;

bool RETRACTING = true;

int extension_count = 0;
int retraction_count = 0;

unsigned long start_time;
unsigned long range_time; 

bool FORCE_THRESHOLD_REACHED(){
  if (MEASURED_FORCE < MAX_FORCE){
    Serial.println("False");
    return false;
  }
  else if (MEASURED_FORCE >= MAX_FORCE) {
    Serial.println("True");
    return true;
  }
}

int x = 0;

HX711 scale;
float calibration_factor = 44240.5708461;

void setup() {
  //Setup
  Serial.begin(9600);
  scale.begin(DT_PIN, SCK_PIN);
  pinMode(RPWM,OUTPUT);
  pinMode(LPWM,OUTPUT); 

  //Calibration
  scale.tare();
  scale.set_scale(calibration_factor);

  //Feedback
  start_time = millis();
}

void loop() {
  MEASURED_FORCE = scale.get_units();
  Serial.println(scale.get_units()); //Display Sensor Force Readings
  bool THRESHOLD = FORCE_THRESHOLD_REACHED(); //Call Threshold Function
  Serial.println("THRESHOLD");
  Serial.println(THRESHOLD); //Result of Threshold Function

  unsigned long current_time = millis(); //Takes current time when executed.

  if (THRESHOLD){ 
    range_time = 100;

    if (RETRACTING){
    retraction_count += 1;
    }
    else {
      extension_count += 1;
    }

  }
  else {
    range_time = 3875;
  }

  if (current_time-start_time >= range_time){
    digitalWrite(RPWM,HIGH);   
    digitalWrite(LPWM,LOW);
    Serial.println(scale.get_units());
    Serial.println("Retracting");
    RETRACTING = true;
    start_time = millis();
  }
  else{
    digitalWrite(RPWM,LOW);   
    digitalWrite(LPWM,HIGH);
    RETRACTING = false;
    Serial.println(scale.get_units());
    Serial.println("Extending");
  }

  Serial.println("extension_count:");
  Serial.println(extension_count);
}

