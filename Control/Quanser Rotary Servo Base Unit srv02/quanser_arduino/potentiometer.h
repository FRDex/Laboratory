#ifndef __POTENTIOMETER_H_INCLUDED__
#define __POTENTIOMETER_H_INCLUDED__
//EXTERNAL LIBRARIES
#include <Arduino.h>

// REFERENCES
#ifndef PI
#define PI (float)(3.14159265358979)
#endif

/*POTENTIOMETER VALUE*/
float potentiometer_get_value();  // VALUE MIGHT BE IN RADIANS/DEGREES DEPENDING ON CONFIG (DEFAULT: RADIANS)
bool potentiometer_is_radians();

/*SETUP*/
bool potentiometer_set(uint8_t pin, bool set_zero_reference = true, bool radians = true, float max_voltage = 5, uint16_t max_resolution = 1024);
bool potentiometer_set_actual_position_to_zero_reference();  // THE REFERENCE VALUE IS SET AT ACTUAL MOTOR POSITION
bool potentiometer_set_zero_reference(uint16_t value);  // THE REFERENCE VALUE IS SET TO A SPECIFIED VALUE
bool potentiometer_set_radians(bool radians = true);

#endif
