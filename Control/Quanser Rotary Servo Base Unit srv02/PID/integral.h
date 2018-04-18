#ifndef __INTEGRAL_H_INCLUDED__
#define __INTEGRAL_H_INCLUDED__

/*INTEGRAL VALUE*/
float integral_get_value();
float integral_new_value(float value, float sample_separation);

/*INTEGRAL SETUP*/
bool integral_setup(uint8_t points, float integral[2][]);
bool integral_clear(uint8_t points, float integral[2][]);

#endif
