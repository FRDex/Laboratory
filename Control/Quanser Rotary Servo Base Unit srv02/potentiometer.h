#ifndef __potentiometer_H_INCLUDED__
#define __potentiometer_H_INCLUDED__

/*POTENTIOMETER VALUE*/
inline uint16_t potentiometer_get_value(); //RETURNS DEGREE VALUE FROM REFERENCE
bool potentiometer_is_radians(bool radians);
/*SETUP */
bool potentiometer_set(uint8_t potentiometer_pin, bool set_zero_reference);
bool potentiometer_set(uint8_t potentiometer_pin, bool set_zero_reference, float voltage, uint16_t resolution, bool radians);
bool potentiometer_set_radians(bool radians);
bool potentiometer_set_zero_reference();

#endif
