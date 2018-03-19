#ifndef __potentiometer_H_INCLUDED__
#define __potentiometer_H_INCLUDED__

/*POTENTIOMETER VALUE*/
inline uint16_t potentiometer_get_value(); //RETURNS DEGREE VALUE FROM REFERENCE

/*SETUP */
bool potentiometer_set(uint8_t potentiometer_pin, bool set_zero_reference);
bool potentiometer_set(uint8_t potentiometer_pin, bool set_zero_reference, float voltage, uint16_t resolution);

#endif
