#include "potentiometer.h"

/*HARDWARE INPUT*/
static uint8_t potentiometer_pin;

/*CONDITIONS*/
static bool potentiometer_is_set = false;
static bool is_radian = true;

/*CALIBRATION VALUES*/
static uint16_t potentiometer_zero_ref = 0;
static float voltage = 5.0;
static uint16_t resolution = 1024;
static float potentiometer_scale = (2 * PI * voltage) / resolution;

// FUNCTIONS DEFINITIONS

/*POTENTIOMETER VALUE*/
float potentiometer_get_value(){
  return (analogRead(potentiometer_pin) - potentiometer_zero_ref) * potentiometer_scale;
}

bool potentiometer_is_radians(){
    return(is_radian);
}

/*SETUP*/
bool potentiometer_set(uint8_t pin, bool set_zero_reference, bool radians, float max_voltage, uint16_t max_resolution){
  potentiometer_pin = pin;
  voltage = max_voltage;
  resolution = max_resolution;
  pinMode(potentiometer_pin, INPUT);
  potentiometer_set_radians(radians);
  if(set_zero_reference){
    potentiometer_set_actual_position_to_zero_reference();
  }
  potentiometer_is_set = true;
  return potentiometer_is_set;
}

bool potentiometer_set_actual_position_to_zero_reference(){
  potentiometer_zero_ref = analogRead(potentiometer_pin);
  return true;
}

bool potentiometer_set_zero_reference(uint16_t value){
  potentiometer_zero_ref = value;
  return true;
}

bool potentiometer_set_radians(bool radians){
    is_radian = radians;
    if(is_radian){
        potentiometer_scale = (2 * PI * voltage) / resolution;
    }
    else{
        potentiometer_scale = (360 * voltage) / resolution;
    }
    return is_radian;
}
