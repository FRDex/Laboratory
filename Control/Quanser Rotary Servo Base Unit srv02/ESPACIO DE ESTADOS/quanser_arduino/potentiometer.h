#ifndef __POTENTIOMETER_H_INCLUDED__
#define __POTENTIOMETER_H_INCLUDED__
//EXTERNAL LIBRARIES
#include <Arduino.h>

// REFERENCES
#ifndef PI
#define PI (float)(3.14159265358979)
#endif

/*POTENTIOMETER VALUE*/
float potentiometer_get_angle();  // VALUE MIGHT BE IN RADIANS/DEGREES DEPENDING ON CONFIG (DEFAULT: RADIANS)
bool potentiometer_is_radians();

/*SETUP*/
static const uint16_t potenciometer_positive_90_degrees_value = 630; 
static const uint16_t potenciometer_0_degrees_value = 512;
static const uint16_t potenciometer_negative_90_degrees_value = 394; 
bool potentiometer_set(uint8_t pin, bool set_zero_reference = false, uint16_t value = potenciometer_0_degrees_value, bool radians = false, uint16_t max_resolution = 1023);
bool potentiometer_set_actual_position_to_zero_reference();  // THE REFERENCE VALUE IS SET AT ACTUAL MOTOR POSITION
bool potentiometer_set_zero_reference_value(uint16_t value);  // THE REFERENCE VALUE IS SET TO A SPECIFIED VALUE
bool potentiometer_set_radians(bool radians = true);

#endif
