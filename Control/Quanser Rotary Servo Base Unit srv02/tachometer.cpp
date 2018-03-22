#include <tachometer.h>

#ifndef __PI_DEFINED__
#define __PI_DEFINED__
  #define PI (float)(3.14159265358979)
#endif

//HARDWARE INPUT
uint8_t tachometer_pin;

//CALIBRATION VALUES
bool tachometer_is_set = false;
uint16_t tachometer_zero_ref = 0;
float voltage = 5.0;
uint16_t resolution = 1024;
bool is_radian = true;
uint16_t tachometer_sensitivity = 1500; // rpm/V
float tachometer_scale = (2 * PI * voltage * tachometer_sensitivity) / 60;


bool tachometer_is_radians(){
    return(is_radian);
}

bool tachometer_set_radians(bool radians){
    is_radian = radians;
    if(is_radian){
        tachometer_scale = (2 * PI * voltage * tachometer_sensitivity) / 60;
    }
    else{
        tachometer_scale = (360 * voltage * tachometer_sensitivity) / 60;
    }

}

bool tachometer_set(uint8_t tachometer_pin, bool set_zero_reference, float voltage, uint16_t resolution, bool radians, uint16_t tachometer_sensitivity){
  this->voltage = voltage;
  this->resolution = resolution;
  this->tachometer_sensitivity = tachometer_sensitivity;
  tachometer_set_radians(radians);
  return tachometer_set(tachometer_pin, set_zero_reference);
}

bool tachometer_set(uint8_t tachometer_pin, bool set_zero_reference){
      this->tachometer_pin = tachometer_pin;
      pinMode(tachometer_pin, INPUT);
      if(set_zero_reference){
        tachometer_set_zero_reference();
      }
      tachometer_is_set = true;
      return tachometer_is_set;
}

bool tachometer_set_zero_reference(){
  tachometer_zero_ref = analogRead(tachometer_pin);
  return true;
}

inline uint16_t tachometer_get_value(){
  return ((analogRead(tachometer_pin) - tachometer_zero_ref) * tachometer_scale) / resolution;
}
