#include <tachometer.h>
// REFERENCES
#ifndef __PI_DEFINED__
  #define __PI_DEFINED__
  #define PI (float)(3.14159265358979)
#endif

/*HARDWARE INPUT*/
static uint8_t tachometer_pin;

/*CONDITIONS*/
static bool tachometer_is_set = false;
static bool is_radian = true;

/*CALIBRATION VALUES*/
static uint16_t tachometer_zero_ref = 0;
static float voltage = 5.0;
static uint16_t resolution = 1024;
static uint16_t tachometer_sensitivity = 1500; // rpm/V
static float tachometer_scale = (2 * PI * voltage * tachometer_sensitivity) / (60 * resolution);

// FUNCTIONS DEFINITIONS

/*TACHOMETER VALUE*/
uint16_t tachometer_get_value(){
  return (analogRead(tachometer_pin) - tachometer_zero_ref) * tachometer_scale;
}

bool tachometer_is_radians(){
    return(is_radian);
}

/*SETUP*/
bool tachometer_set(uint8_t tachometer_pin, bool set_zero_reference, bool radians, float voltage, uint16_t resolution, uint16_t tachometer_sensitivity){
  this->tachometer_pin = tachometer_pin;
  this->voltage = voltage;
  this->resolution = resolution;
  this->tachometer_sensitivity = tachometer_sensitivity;
  pinMode(tachometer_pin, INPUT);
  tachometer_set_radians(radians);
  if(set_zero_reference){
    tachometer_set_zero_reference();
  }
  tachometer_is_set = true;
  return tachometer_is_set;
}

bool tachometer_set(uint8_t tachometer_pin, bool set_zero_reference){
      return tachometer_set(tachometer_pin, set_zero_reference, true, 5.0, 1024, 1500);
}

bool tachometer_set_zero_reference(){
  tachometer_zero_ref = analogRead(tachometer_pin);
  return true;
}

bool tachometer_set_zero_reference(uint16_t value){
  tachometer_zero_ref = value;
  return true;
}

bool tachometer_set_radians(bool radians){
    is_radian = radians;
    if(is_radian){
        tachometer_scale = (2 * PI * voltage * tachometer_sensitivity) / (60 * resolution);
    }
    else{
        tachometer_scale = (360 * voltage * tachometer_sensitivity) / (60 * resolution);
    }
    return is_radian;
}
