#ifndef __TACHOMETER_H_INCLUDED__
#define __TACHOMETER_H_INCLUDED__
//EXTERNAL LIBRARIES
#include <Arduino.h>

// REFERENCES
#ifndef PI
#define PI (float)(3.14159265358979)
#endif

/*TACHOMETER VALUE*/
float tachometer_get_value();  // VALUE MIGHT BE IN RADIANS/DEGREES DEPENDING ON CONFIG (DEFAULT: RADIANS)
bool tachometer_is_radians();

/*SETUP */
bool tachometer_set(uint8_t pin, bool set_zero_reference, bool radians, float max_voltage, uint16_t max_resolution, uint16_t sensitivity);
bool tachometer_set(uint8_t tachometer_pin, bool set_zero_reference);  // ASSUMES DEFAULT VALUES (true, 5V, 1024, 1500)
bool tachometer_set_zero_reference();  // THE REFERENCE VALUE IS SET AT ACTUAL MOTOR STATE
bool tachometer_set_zero_reference(uint16_t value);  // THE REFERENCE VALUE IS SET TO A SPECIFIED VALUE
bool tachometer_set_radians(bool radians);

#endif
