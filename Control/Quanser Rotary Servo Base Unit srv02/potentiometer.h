#ifndef __potentiometer_H_INCLUDED__
#define __potentiometer_H_INCLUDED__

/*POTENTIOMETER VALUE*/
inline uint16_t potentiometer_get_value();  // VALUE MIGHT BE IN RADIANS/DEGREES DEPENDING ON CONFIG (DEFAULT: RADIANS)
bool potentiometer_is_radians(bool radians);

/*SETUP*/
bool potentiometer_set(uint8_t potentiometer_pin, bool set_zero_reference, float voltage, uint16_t resolution, bool radians);
bool potentiometer_set(uint8_t potentiometer_pin, bool set_zero_reference);  // ASSUMES DEFAULT VALUES (5V, 1024, true) 
bool potentiometer_set_zero_reference();  // THE REFERENCE VALUE IS SET AT ACTUAL MOTOR POSITION
bool potentiometer_set_zero_reference(uint16_t value);  // THE REFERENCE VALUE IS SET TO A SPECIFIED VALUE
bool potentiometer_set_radians(bool radians);

#endif
