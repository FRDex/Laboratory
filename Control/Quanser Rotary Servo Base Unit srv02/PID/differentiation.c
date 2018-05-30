#include <differentiation.h>

/*FUNCTION DECLARATION*/
void shift_values(differentiation_struct & differentiation_structure);

/*DIFFENTIATIAL FUNCTIONS*/
float differential_get_value(differentiation_struct & differentiation_structure){
  return differentiation_structure.derivative;
}

//NUMERICAL FORWARD DIFFENTIATION TO APROXIMATE THE DERIVATIVE
float differential_new_value(float value, float sample_separation, differentiation_struct & differentiation_structure){
  differentiation_structure.differential_degree = (differentiation_structure.differential_degree > differentiation_structure.differential_points) ? differentiation_structure.differential_points : differentiation_structure.differential_degree;

  switch (differentiation_structure.differential_degree) {
    case 0:{
      differentiation_structure.derivative = 0;
    break;
    }
    case 1:{
      differentiation_structure.derivative = (value - differentiation_structure.previous_value) / sample_separation;
    break;
    }
    case 2:{
      // FACTORS FOR CALCULATE THE DERIVATIVE
      static const float factor_2 = 1.0 / 3.0;

      differentiation_structure.derivative = 2.0 * (value - differentiation_structure.previous_value) / sample_separation;
      differentiation_structure.derivative += differentiation_structure.differential_values[1];
      differentiation_structure.derivative *= factor_2;
    }
    case 3:{
      // FACTORS FOR CALCULATE THE DERIVATIVE
      static const float factor_3 = 1.0 / 8.0;
      float sample_separation_ratio_1 = sample_separation / differentiation_structure.differential_separation[1];

      differentiation_structure.derivative = 6.0 * (value - differentiation_structure.previous_value) / sample_separation;
      differentiation_structure.derivative += (2 - sample_separation_ratio_1) * differentiation_structure.differential_values[1];
      differentiation_structure.derivative += sample_separation_ratio_1 * differentiation_structure.differential_values[2];
      differentiation_structure.derivative *= factor_3;
    break;
    }
    case 4: default:{
      // FACTORS FOR CALCULATE THE DERIVATIVE
      static const float factor_4 = 1.0 / 33.0;
      float sample_separation_ratio_1 = sample_separation / differentiation_structure.differential_separation[1];
      float sample_separation_ratio_1_1 = sample_separation_ratio_1 * sample_separation_ratio_1;
      float sample_separation_ratio_2 = sample_separation / differentiation_structure.differential_separation[2];
      float sample_separation_ratio_1_2 = sample_separation_ratio_1 * sample_separation_ratio_2;

      differentiation_structure.derivative = 24.0 * (value - differentiation_structure.previous_value) / sample_separation;
      differentiation_structure.derivative += (9 - 3 * sample_separation_ratio_1 + sample_separation_ratio_1_1) * differentiation_structure.differential_values[1];
      differentiation_structure.derivative += (3 * sample_separation_ratio_1 - sample_separation_ratio_1_1 - sample_separation_ratio_1_2) * differentiation_structure.differential_values[2];
      differentiation_structure.derivative += sample_separation_ratio_1_2 * differentiation_structure.differential_values[2];
      differentiation_structure.derivative *= factor_4;
    break;
    }
  }
  differentiation_structure.differential_separation[0] = sample_separation;
  differentiation_structure.differential_values[0] = derivative;
  differentiation_structure.previous_value = value;
  ++differentiation_structure.differential_degree;
  shift_values(differentiation_structure);

  return differentiation_structure.derivative;
}

/*DIFFENTIATIAL SETUP*/
bool differential_setup(uint8_t differential_points, differentiation_struct & differentiation_structure){
  is_differential_set = false;
  if(differential_points > 1){
    differentiation_structure.differential_points = differential_points;
    is_differential_set = differential_clear(differentiation_structure);
  }
	return is_differential_set;
}

bool differential_clear(differentiation_struct & differentiation_structure){
  differentiation_structure.differential_points = (differential_points >= DIFFERENTIATION_MIN_POINTS) ? differentiation_structure.differential_points : DIFFERENTIATION_MIN_POINTS;
  differentiation_structure.differential_points = (differentiation_structure.differential_points >= DIFFERENTIATION_MAX_POINTS) ? DIFFERENTIATION_MAX_POINTS : differentiation_structure.differential_points;

  differentiation_structure.differential_degree = 0;
  differentiation_structure.previous_value = 0
  for(uint8_t k = 0; k < differentiation_structure.differential_points; ++k){
    differentiation_structure.differential_values[k] = 0;
    differentiation_structure.differential_separation[k] = 0;
  }
  return true;
}

void shift_values(differentiation_struct & differentiation_structure){
  for(uint8_t actual_index = differentiation_structure.differential_points - 2, new_index = differentiation_structure.differential_points - 1; new_index > 0; --actual_index, --new_index){
    differentiation_structure.differential_values[new_index] = differentiation_structure.differential_values[actual_index];
    differentiation_structure.differential_separation[new_index] = differentiation_structure.differential_separation[actual_index];
  }
}


/*IT INCREASE INDEXES TO AVOID SHIFTING THE ARRAY */
/* ANOTHER IMPLEMENTATION TO AVOID SHIFTING, BUT IN THIS CASE I THINK THAT
IT WILL BE CLEARER TO SHIFT ARRAYS
//static uint8_t differential_index[DIFFERENTIATION_MAX_POINTS];
void increase_indexes(){
  uint8_t new_index =  differential_index[0] + 1;
  new_index = (new_index >= DIFFERENTIATION_MAX_POINTS) ? 0 : new_index;
  for (uint8_t k = 0, index = new_index; k < differential_points; ++k, --index){
    if(index < 0){
      index = differential_points - 1;
    }
    differential_index[k] = index;
  }
}
*/
