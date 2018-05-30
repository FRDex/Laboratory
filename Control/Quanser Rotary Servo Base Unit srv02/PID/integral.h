#ifndef __INTEGRAL_H_INCLUDED__
#define __INTEGRAL_H_INCLUDED__

/*INTEGRAL STRUCT*/
typedef struct {
  bool is_integral_set;
  /*ARRAY STRUCTURE INFO*/
  uint8_t actual_index, previous_index, n_integral_points;
  float & integral_differential_value;
  /*INTEGRAL VARIABLES*/
  float integral_sum;
  float previous_value;
} integral_struct;

/*INTEGRAL VALUE*/
float integral_get_value(integral_struct &integral_structure);
float integral_new_value(float value, float sample_separation, integral_struct &integral_structure);

/*INTEGRAL SETUP*/
bool integral_setup(uint8_t integral_points, integral_struct &integral_structure);
bool integral_clear(integral_struct &integral_structure);

#endif
