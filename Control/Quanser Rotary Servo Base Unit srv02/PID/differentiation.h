#ifndef __DIFFERENTIATION_H_INCLUDED__
#define __DIFFERENTIATION_H_INCLUDED__

/*INTEGRAL VALUE*/
float differential_get_value();
float differential_new_value(float value, float sample_separation);
/*INTEGRAL SETUP*/
bool differential_setup(uint8_t differential_points);
bool differential_clear(uint8_t differential_points);

#endif
