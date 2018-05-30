#include <differentiation.h>
/*DEFINES*/
define MAX_POINTS 4
define MIN_POINTS 1
/*CONDITIONALS*/
static bool is_differential_set = false;
/*ARRAY STRUCTURE INFO*/
static float last_value = 0
/*DIFFENTIATIAL VARIABLES*/
static uint8_t n_differential_points;
static uint8_t differential_degree;
static float derivative;

static float differential_values[MAX_POINTS];
static float differential_separation[MAX_POINTS];
//static uint8_t differential_index[MAX_POINTS];

/*FUNCTION DECLARATION*/
void shift_values();
//void increase_indexes();

/*DIFFENTIATIAL FUNCTIONS*/
float differential_get_value(){
  return derivative;
}

//NUMERICAL FORWARD DIFFENTIATION TO APROXIMATE THE DERIVATIVE
float differential_new_value(float value, float sample_separation){
  differential_degree = (differential_degree > n_differential_points) ? n_differential_points : differential_degree;

  switch (differential_degree) {
    case 0:{
      derivative = 0;
    break;
    }
    case 1:{
      derivative = (value - last_value) / sample_separation;
    break;
    }
    case 2:{
      // FACTORS FOR CALCULATE THE DERIVATIVE
      static const float factor_2 = 1.0 / 3.0;

      derivative = 2.0 * (value - last_value) / sample_separation;
      derivative += differential_values[1];
      derivative *= factor_2;
    }
    case 3:{
      // FACTORS FOR CALCULATE THE DERIVATIVE
      static const float factor_3 = 1.0 / 8.0;
      float sample_separation_ratio_1 = sample_separation / differential_separation[1];

      derivative = 6.0 * (value - last_value) / sample_separation;
      derivative += (2 - sample_separation_ratio_1) * differential_values[1];
      derivative += sample_separation_ratio_1 * differential_values[2];
      derivative *= factor_3;
    break;
    }
    case 4: default:{
      // FACTORS FOR CALCULATE THE DERIVATIVE
      static const float factor_4 = 1.0 / 33.0;
      float sample_separation_ratio_1 = sample_separation / differential_separation[1];
      float sample_separation_ratio_1_1 = sample_separation_ratio_1 * sample_separation_ratio_1;
      float sample_separation_ratio_2 = sample_separation / differential_separation[2];
      float sample_separation_ratio_1_2 = sample_separation_ratio_1 * sample_separation_ratio_2;

      derivative = 24.0 * (value - last_value) / sample_separation;
      derivative += (9 - 3 * sample_separation_ratio_1 + sample_separation_ratio_1_1) * differential_values[1];
      derivative += (3 * sample_separation_ratio_1 - sample_separation_ratio_1_1 - sample_separation_ratio_1_2) * differential_values[2];
      derivative += sample_separation_ratio_1_2 * differential_values[2];
      derivative *= factor_4;
    break;
    }
  }
  differential_separation[0] = sample_separation;
  differential_values[0] = derivative;
  last_value = value;
  ++differential_degree;
  shift_values();
  //increase_indexes();

  return derivative;
}

/*DIFFENTIATIAL SETUP*/
bool differential_setup(uint8_t differential_points){
  is_differential_set = false;
  if(differential_points > 1){
    is_differential_set = differential_clear(differential_points);
  }
	return is_differential_set;
}

bool differential_clear(uint8_t differential_points){
  n_differential_points = (differential_points >= MIN_POINTS) ? differential_points : MIN_POINTS;
  n_differential_points = (n_differential_points >= MAX_POINTS) ? MAX_POINTS : n_differential_points;
  differential_degree = 0;
  last_value = 0
  for(uint8_t k = 0; k < n_differential_points; ++k){
    differential_values[k] = 0;
    differential_separation[k] = 0;
    //differential_index[k] = n_differential_points - 1 - k;
  }
  return true;
}

void shift_values(){
  for(uint8_t actual_index = n_differential_points - 2, new_index = n_differential_points - 1; new_index > 0; --actual_index, --new_index){
    differential_values[new_index] = differential_values[actual_index];
    differential_separation[new_index] = differential_separation[actual_index];
  }
}

/*IT INCREASE INDEXES TO AVOID SHIFTING THE ARRAY */
/* ANOTHER IMPLEMENTATION TO AVOID SHIFTING, BUT IN THIS CASE I THINK THAT
IT WILL BE CLEARER TO SHIFT ARRAYS
void increase_indexes(){
  uint8_t new_index =  differential_index[0] + 1;
  new_index = (new_index >= MAX_POINTS) ? 0 : new_index;
  for (uint8_t k = 0, index = new_index; k < n_differential_points; ++k, --index){
    if(index < 0){
      index = n_differential_points - 1;
    }
    differential_index[k] = index;
  }
}
*/
