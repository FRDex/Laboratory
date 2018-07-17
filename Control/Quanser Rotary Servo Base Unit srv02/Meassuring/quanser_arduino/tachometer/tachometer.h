#ifndef __TACHOMETER_H_INCLUDED__
#define __TACHOMETER_H_INCLUDED__
//EXTERNAL LIBRARIES
#include <Arduino.h>

// REFERENCES
#ifndef PI
#define PI (float)(3.14159265358979)
#endif

/*TACHOMETER VALUE*/
float tachometer_get_value();  // VALUE MIGHT BE IN radianes/DEGREES DEPENDING ON CONFIG (DEFAULT: radianes)
bool tachometer_is_radianes();

/*SETUP */
bool tachometer_set(uint8_t pin, bool set_zero_reference = true, bool radianes = true, float max_voltage = 5, uint16_t max_resolution = 1024, uint16_t sensitivity = 1500);
bool tachometer_set_actual_value_to_zero_reference();  // THE REFERENCE VALUE IS SET AT ACTUAL MOTOR STATE
bool tachometer_set_zero_reference(uint16_t value = 0);  // THE REFERENCE VALUE IS SET TO A SPECIFIED VALUE
bool tachometer_set_radianes(bool radianes = true);

#endif
