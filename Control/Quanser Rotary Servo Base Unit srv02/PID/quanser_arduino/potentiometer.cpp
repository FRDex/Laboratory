#include "potentiometer.h"

/*HARDWARE INPUT*/
static uint8_t potentiometer_pin;

/*CONDITIONS*/
static bool potentiometer_is_set = false;
static bool is_radian = true;

/*CALIBRATION VALUES*/
static uint16_t potentiometer_zero_ref = 0;
static uint16_t resolution = 1023;


static float potentiometer_scale = PI / ((float)(potenciometer_positive_90_degrees_value - potenciometer_negative_90_degrees_value));

// FUNCTIONS DEFINITIONS

/*POTENTIOMETER VALUE*/
float potentiometer_get_angle(){
  return ((int16_t)(analogRead(potentiometer_pin)) - (int16_t)(potentiometer_zero_ref)) * potentiometer_scale;
}

bool potentiometer_is_radians(){
    return(is_radian);
}

/*SETUP*/
bool potentiometer_set(uint8_t pin, bool set_zero_reference, uint16_t value, bool radians, uint16_t max_resolution){
  potentiometer_pin = pin;
  pinMode(potentiometer_pin, INPUT);
  resolution = max_resolution;
  potentiometer_set_radians(radians);
  if(set_zero_reference){
    potentiometer_set_actual_position_to_zero_reference();
  }
  else{
    potentiometer_set_zero_reference_value(value);
  }
  potentiometer_is_set = true;
  return potentiometer_is_set;
}

bool potentiometer_set_actual_position_to_zero_reference(){
  potentiometer_zero_ref = analogRead(potentiometer_pin);
  return true;
}

bool potentiometer_set_zero_reference_value(uint16_t value){
  potentiometer_zero_ref = value;
  return true;
}

bool potentiometer_set_radians(bool radians){
    is_radian = radians;
    if(is_radian){
        potentiometer_scale = PI / ((float)(potenciometer_positive_90_degrees_value - potenciometer_negative_90_degrees_value));
    }
    else{
        potentiometer_scale = 180.0  / ((float)(potenciometer_positive_90_degrees_value - potenciometer_negative_90_degrees_value));
    }
    return is_radian;
}
