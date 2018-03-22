#ifndef __tachometer_H_INCLUDED__
#define __tachometer_H_INCLUDED__

/*tachometer VALUE*/
inline uint16_t tachometer_get_value(); //RETURNS DEGREE VALUE FROM REFERENCE
bool tachometer_is_radians(bool radians);
/*SETUP */
bool tachometer_set(uint8_t tachometer_pin, bool set_zero_reference);
bool tachometer_set(uint8_t tachometer_pin, bool set_zero_reference, float voltage, uint16_t resolution, bool radians);
bool tachometer_set_radians(bool radians);

#endif
