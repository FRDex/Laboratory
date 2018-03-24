#include <potentiometer.h>
// REFERENCES
#ifndef __PI_DEFINED__
#define __PI_DEFINED__
  #define PI (float)(3.14159265358979)
#endif

/*HARDWARE INPUT*/
uint8_t potentiometer_pin;

/*CONDITIONS*/
bool potentiometer_is_set = false;
bool is_radian = true;

/*CALIBRATION VALUES*/
uint16_t potentiometer_zero_ref = 0;
float voltage = 5.0;
uint16_t resolution = 1024;
float potentiometer_scale = (2 * PI * voltage * tachometer_sensitivity) / resolution;

// FUNCTIONS DEFINITIONS

/*POTENTIOMETER VALUE*/
uint16_t potentiometer_get_value(){
  return (analogRead(potentiometer_pin) - potentiometer_zero_ref) * potentiometer_scale;
}

bool potentiometer_is_radians(){
    return(is_radian);
}

/*SETUP*/
bool potentiometer_set(uint8_t potentiometer_pin, bool set_zero_reference, bool radians, float voltage, uint16_t resolution){
  this->potentiometer_pin = potentiometer_pin;
  this->voltage = voltage;
  this->resolution = resolution;
  pinMode(potentiometer_pin, INPUT);
  potentiometer_set_radians(radians);
  if(set_zero_reference){
    potentiometer_set_zero_reference();
  }
  potentiometer_is_set = true;
  return potentiometer_is_set;
}

bool potentiometer_set(uint8_t potentiometer_pin, bool set_zero_reference){
      return potentiometer_set(potentiometer_pin, set_zero_reference, true, 5.0, 1024);
}

bool potentiometer_set_zero_reference(){
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
