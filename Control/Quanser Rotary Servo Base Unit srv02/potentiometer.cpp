#include <potentiometer.h>

#define PI (float)(3.14159265358979)
//HARDWARE INPUT
uint8_t potentiometer_pin;

//CALIBRATION VALUES
bool potentiometer_is_set = false;
uint16_t potentiometer_zero_ref = 0;
float voltage = 5.0;
uint16_t resolution = 1024;
bool is_radian = true;
float potentiometer_scale = 2 * PI * voltage;


bool potentiometer_is_radians(){
    return(is_radian);
}

bool potentiometer_set_radians(bool radians){
    is_radian = radians;
    if(is_radian){
        potentiometer_scale = 2 * PI * voltage;
    }
    else{
        potentiometer_scale = 360 * voltage;
    }

}

bool potentiometer_set(uint8_t potentiometer_pin, bool set_zero_reference, float voltage, uint16_t resolution, bool radians){
  this->voltage = voltage;
  this->resolution = resolution;
  potentiometer_set_radians(radians);
  return potentiometer_set(potentiometer_pin, set_zero_reference);
}

bool potentiometer_set(uint8_t potentiometer_pin, bool set_zero_reference){
      this->potentiometer_pin = potentiometer_pin;
      pinMode(potentiometer_pin, INPUT);
      if(set_zero_reference){
        set_zero_reference();
      }
      potentiometer_is_set = true;
      return potentiometer_is_set;
}

bool set_zero_reference(){
  potentiometer_zero_ref = analogRead(PIN_POTE);
  return true;
}

inline uint16_t potentiometer_get_value(){
  return ((analogRead(PIN_POTE) - potentiometer_zero_ref) * voltage * potentiometer_scale) / resolution;
}
