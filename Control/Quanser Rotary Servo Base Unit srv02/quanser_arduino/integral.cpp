#include "integral.h"
static bool is_integral_set = false;
/*FUNCTION DECLARATION*/
static void increase_indexes(integral_struct &integral_structure);

/*INTEGRAL FUNCTIONS*/
float integral_get_value(integral_struct &integral_structure){
  return integral_structure.integral_sum;
}

//THE COMPOSITE NUMERICAL INTEGRATION METHOD IS IMPLEMENT TO APROXIMATE THE INTEGRAL
//IT CAN BE DEMONSTRATED THAT THE COMPOSITE NUMERICAL INTEGRATION IS EQUIVALENT TO TRAPEZOIDAL INTEGRATION METHOD
//THIS METHOD IS FOR INTEGRAL OF THE LAST N ELEMENTS
float integral_new_value(float value, float sample_separation, integral_struct &integral_structure){
  if(is_integral_set){
    integral_structure.integral_sum -= integral_structure.integral_differential_value[integral_structure.index];  // SUBSTRACT THE LAST SAMPLE THAT WILL BE OVERWRITE
    integral_structure.integral_differential_value[integral_structure.index] = ((value + integral_structure.previous_value) * sample_separation) / 2;
    integral_structure.integral_sum += integral_structure.integral_differential_value[integral_structure.index];
    integral_structure.previous_value = value;
    increase_indexes(integral_structure);
  }
  return integral_structure.integral_sum;
}

/*INTEGRAL SETUP*/
bool integral_setup(uint8_t integral_points, integral_struct &integral_structure, float *error_integral_array){
  is_integral_set = false;
  if(integral_points > 1){
    integral_structure.integral_points = integral_points;
    is_integral_set = integral_clear(integral_structure, error_integral_array);
  }
  return is_integral_set;
}

bool integral_clear(integral_struct &integral_structure, float *error_integral_array){
  // INTEGRAL PARAMETERS
  integral_structure.index = 1;
  integral_structure.previous_value = 0.0;
  integral_structure.integral_sum = 0.0;
  integral_structure.integral_differential_value = error_integral_array;
  for(uint8_t k = 0; k < integral_structure.integral_points; ++k){
     integral_structure.integral_differential_value[k] = 0;
  }
  integral_structure.is_integral_set = true;
  return integral_structure.is_integral_set;
}

void increase_indexes(integral_struct &integral_structure){
  ++integral_structure.index;
  if(integral_structure.index >= integral_structure.integral_points){
    integral_structure.index = 0;
  }
}
