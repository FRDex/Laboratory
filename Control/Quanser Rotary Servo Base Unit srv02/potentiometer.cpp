#include <potentiometer.h>
uint8_t potentiometer_pin;
uint16_t potentiometer_zero_ref = 0;
bool potentiometer_is_set = false;
float voltage = 5.0;
uint16_t resolution = 1024;

bool potentiometer_set(uint8_t potentiometer_pin, bool set_zero_reference, float voltage, uint16_t resolution){
  this->voltage = voltage;
  this->resolution = resolution;
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

bool default_case(){
  return (voltage == 5.0) && (resolution == 1024);
}

inline uint16_t potentiometer_get_value(){
    if (default_case()){
        //(voltage_potentiometer * voltage / resolution) * 360
        //(voltage_potentiometer * 5 / 1024) * 360
        return ((analogRead(PIN_POTE) - potentiometer_zero_ref) * 1800) >> 10;
    }
    else{
      return ((analogRead(PIN_POTE) - potentiometer_zero_ref) * voltage * 360) / resolution;
    }
}
