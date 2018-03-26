#ifndef __POTENTIOMETER_H_INCLUDED__
#define __POTENTIOMETER_H_INCLUDED__

/*POTENTIOMETER VALUE*/
uint16_t potentiometer_get_value();  // VALUE MIGHT BE IN RADIANS/DEGREES DEPENDING ON CONFIG (DEFAULT: RADIANS)
bool potentiometer_is_radians();

/*SETUP*/
bool potentiometer_set(uint8_t pin, bool set_zero_reference, bool radians, float max_voltage, uint16_t max_resolution);
bool potentiometer_set(uint8_t pin, bool set_zero_reference);  // ASSUMES DEFAULT VALUES (true, 5V, 1024)
bool potentiometer_set_zero_reference();  // THE REFERENCE VALUE IS SET AT ACTUAL MOTOR POSITION
bool potentiometer_set_zero_reference(uint16_t value);  // THE REFERENCE VALUE IS SET TO A SPECIFIED VALUE
bool potentiometer_set_radians(bool radians);

#endif
