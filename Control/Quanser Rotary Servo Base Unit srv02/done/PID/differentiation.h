#ifndef __DIFFERENTIATION_H_INCLUDED__
#define __DIFFERENTIATION_H_INCLUDED__

/*DEFINES*/
define DIFFERENTIATION_MAX_POINTS 4
define DIFFERENTIATION_MIN_POINTS 1

/*DIFFERENTIATION STRUCT*/
typedef struct {
  /*CONDITIONALS*/
  bool is_differential_set;
  /*DIFFERENTIATION VARIABLES*/
  uint8_t differential_degree, differential_points;
  /*ARRAY STRUCTURE INFO*/
  float actual_taylor_elements[DIFFERENTIATION_MAX_POINTS];
  float previous_taylor_elements[DIFFERENTIATION_MAX_POINTS];
} differentiation_struct;

/*INTEGRAL VALUE*/
float differential_get_value(differentiation_struct & differentiation_structure);
float differential_new_value(float value, float sample_separation, differentiation_struct & differentiation_structure);
/*INTEGRAL SETUP*/
bool differential_setup(uint8_t differential_points, differentiation_struct & differentiation_structure);
bool differential_clear(differentiation_struct & differentiation_structure);

#endif
