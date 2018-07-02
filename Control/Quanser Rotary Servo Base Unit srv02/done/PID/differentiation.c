#include <differentiation.h>

/*FUNCTION DECLARATION*/
void shift_values(differentiation_struct & differentiation_structure);

/*DIFFENTIATIAL FUNCTIONS*/
float differential_get_value(differentiation_struct & differentiation_structure){
  return differentiation_structure.actual_taylor_elements[1];
}


define ORDER_1(index, sample_separation, actual_values, previous_values) (actual_values[index - 1] - previous_values[index - 1]) / sample_separation
define ORDER_2(index, sample_separation, actual_values, previous_values)  2 * ORDER_1(index, sample_separation, actual_values, previous_values) - previous_values[index]
define ORDER_3(index, sample_separation, actual_values, previous_values)  3 * ORDER_1(index, sample_separation, actual_values, previous_values) - 2 * previous_values[index] - 0.5 * previous_values[index + 1] * sample_separation
define ORDER_4(index, sample_separation, actual_values, previous_values)  4 * ORDER_1(index, sample_separation, actual_values, previous_values) - 3 * previous_values[index] - previous_values[index + 1] * sample_separation + previous_values[index + 2] * sample_separation * sample_separation / 6


//NUMERICAL FORWARD DIFFENTIATION TO APROXIMATE THE DERIVATIVE
float differential_new_value(float value, float sample_separation, differentiation_struct & differentiation_structure){
  differentiation_structure.differential_degree = (differentiation_structure.differential_degree > differentiation_structure.differential_points) ? differentiation_structure.differential_points : differentiation_structure.differential_degree;

  differentiation_structure.actual_taylor_elements[0] = value;
  switch (differentiation_structure.differential_degree) {
    case 0:{
      // ONLY LOADS THE PREVIOUS VALUE
    break;
    }
    case 1:{
      differentiation_structure.actual_taylor_elements[1] = ORDER_1(1, sample_separation, differentiation_structure.actual_taylor_elements, differentiation_structure.previous_taylor_elements);
    break;
    }
    case 2:{
      differentiation_structure.actual_taylor_elements[1] = ORDER_2(1, sample_separation, differentiation_structure.actual_taylor_elements, differentiation_structure.previous_taylor_elements);
      differentiation_structure.actual_taylor_elements[2] = ORDER_1(2, sample_separation, differentiation_structure.actual_taylor_elements, differentiation_structure.previous_taylor_elements);
    }
    case 3:{
      differentiation_structure.actual_taylor_elements[1] = ORDER_3(1, sample_separation, differentiation_structure.actual_taylor_elements, differentiation_structure.previous_taylor_elements);
      differentiation_structure.actual_taylor_elements[2] = ORDER_2(2, sample_separation, differentiation_structure.actual_taylor_elements, differentiation_structure.previous_taylor_elements);
      differentiation_structure.actual_taylor_elements[3] = ORDER_1(3, sample_separation, differentiation_structure.actual_taylor_elements, differentiation_structure.previous_taylor_elements);
    break;
    }
    case 4: default:{
      differentiation_structure.actual_taylor_elements[1] = ORDER_4(1, sample_separation, differentiation_structure.actual_taylor_elements, differentiation_structure.previous_taylor_elements);
      differentiation_structure.actual_taylor_elements[2] = ORDER_3(2, sample_separation, differentiation_structure.actual_taylor_elements, differentiation_structure.previous_taylor_elements);
      differentiation_structure.actual_taylor_elements[3] = ORDER_2(3, sample_separation, differentiation_structure.actual_taylor_elements, differentiation_structure.previous_taylor_elements);
      //differentiation_structure.actual_taylor_elements[4] = ORDER_1(4, sample_separation, differentiation_structure.actual_taylor_elements, differentiation_structure.previous_taylor_elements);
    break;
    }
  }
  ++differentiation_structure.differential_degree;
  // UPDATE PREVIOUS VALUES
  for(uint8_t k = 0; k < differentiation_structure.differential_degree; ++k){
    differentiation_structure.previous_taylor_elements[k] = differentiation_structure.actual_taylor_elements[k];
  }
  return differentiation_structure.actual_taylor_elements[1];
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
  return true;
}
