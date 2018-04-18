#include <integral.h>
/*CONDITIONALS*/
static bool is_integral_set = false;
/*ARRAY STRUCTURE INFO*/
static enum {sample_value = 0, differential_value = 1}
static uint8_t actual_index;
static uint8_t previous_index;
/*INTEGRAL VARIABLES*/
static uint8_t n_integral_points;
static float integral_value = 0.0;

/*FUNCTION DECLARATION*/
void increase_indexes();

/*INTEGRAL FUNCTIONS*/
float integral_get_value(){
  return integral_value;
}

//THE COMPOSITE NUMERICAL INTEGRATION METHOD IS IMPLEMENT TO APROXIMATE THE INTEGRAL
//IT CAN BE DEMONSTRATED THAT THE COMPOSITE NUMERICAL INTEGRATION IS EQUIVALENT TO TRAPEZOIDAL INTEGRATION METHOD
//THIS METHOD IS FOR INTEGRAL OF THE LAST N ELEMENTS
float integral_new_value(float value, float sample_separation){
  if(is_integral_set){
    integral_value -= integral[differential_value][actual_index];
    integral[value][k] = value;
    integral[differential_value][k] = ((integral[value][actual_index] + integral[value][previous_index]) * sample_separation) / 2;
    integral_value += integral[differential_value][actual_index];
    increase_indexes();
  }
  return integral_value;
}

/*INTEGRAL SETUP*/
bool integral_setup(uint8_t points, float integral[2][]){
  is_integral_set = false;
  if(points > 1){
    is_integral_set = integral_clear(points, integral);
  }
	return is_integral_set;
}

bool integral_clear(uint8_t points, float integral[2][]){
  n_integral_points = points;
  actual_index = 1;
  previous_index = 0;
  integral_value = 0.0;
  for(uint8_t k = 0; k < points; ++k){
    integral[value][k] = 0;
    integral[differential_value][k] = 0;
  }
  return true;
}

void increase_indexes(){
  previous_index = actual_index;
  ++actual_index;
  if(actual_index >= n_integral_points){
    actual_index = 0;
  }
}
