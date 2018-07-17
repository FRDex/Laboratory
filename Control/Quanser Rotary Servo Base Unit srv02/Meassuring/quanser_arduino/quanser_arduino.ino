#include <Stdint.h>
#include "quadrature_encoder.h"
#include "quanser_motor.h"
#include "potentiometer.h"
//#include "quanser_samples.h"

//#include "differentiation.h"
//#include "integral.h"

static uint8_t encoder_clockwise_pin = 2, encoder_counterclockwise_pin = 3;
static uint8_t motor_clockwise_pin = 6, motor_counterclockwise_pin = 5;
static uint8_t potentiometer_pin = A0;

static const float counter_to_radians = PI / 2048.0;
static const float counters_to_degrees = 180.0 / 2048.0;
static const float degrees_to_counters = 2048.0 / 180.0;
static const float degrees_to_radians = PI / 180.0;
static int16_t angle_reference = 180.0 * degrees_to_radians / counter_to_radians;
static uint8_t timer_prescaller_value = 1;

bool setup_system_to_zero_position();
bool timer_setup(bool active_noise_canceler = false, uint8_t prescaller_value = 1);
int16_t read_integers();



// PID
//#define INTEGRAL_POINTS 5
//#define DIFFERENTIAL_POINTS 2 // MIN 1 MAX 4
//
//static uint8_t error_integral_points = INTEGRAL_POINTS;
//static float error_integral_array [INTEGRAL_POINTS];
//static integral_struct error_integral;

//static uint8_t error_differentiation_points = DIFFERENTIAL_POINTS;
//static differentiation_struct error_differentiation;


void setup() {
  Serial.begin(9600);
  Serial.flush();
  String setup_conditions[2] = {"False", "True"};

  Serial.print("Motor Setup is correct? ");
  Serial.println(setup_conditions[motor_set_pins(motor_clockwise_pin, motor_counterclockwise_pin)]);

  Serial.print("Potentiometer Setup is correct? ");
  Serial.println(setup_conditions[potentiometer_set(potentiometer_pin)]);

  Serial.print("System is set to zero position? ");
  Serial.println(setup_conditions[setup_system_to_zero_position()]);

  Serial.print("Encoder Setup is correct? ");
  Serial.println(setup_conditions[encoder_set(encoder_clockwise_pin, encoder_counterclockwise_pin)]);

  Serial.print("Encoder set zero reference is complete? ");
  Serial.println(setup_conditions[encoder_set_reference_count_value(angle_reference)]);
  angle_reference = 0;
  
  Serial.print("Activation of Encoder is complete? ");
  Serial.println(setup_conditions[encoder_activate()]);

  Serial.print("Timer Setup is correct? ");
  Serial.println(setup_conditions[timer_setup(false, timer_prescaller_value)]);
  TCNT5 = 0;


//  Serial.print("Setup Quanser Samples is complete? ");
//  Serial.println(setup_conditions[setup_quanser_samples()]);

Serial.println("Envia un numero para cambiar las constantes");
  
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////// - VARIABLES INICIALES - ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    static const float timer_frequency = 16e6;
    
    // MOTOR VARIABLES
    static uint8_t max_power_output = 255; // 5V
    static uint8_t min_power_output = 30;  // 0.59V

    // ERROR TOLERANCE
    static int16_t min_positive_angle_tolerance = (1.0 * (float)(degrees_to_counters));
    static int16_t min_negative_angle_tolerance = - min_positive_angle_tolerance;

    // PID GAINS
    static float proportional_gain = 500 * counter_to_radians;
    static float differential_gain = 10.0 * counter_to_radians * timer_frequency / timer_prescaller_value;
    
void loop() {
  
  // PID VARIABLES
    // TIMER FOR DERIVATIVE
    
    static uint16_t time_separation = 0;
    // ERROR
    static int16_t error = 0;
    static int16_t prev_error = 0;
    static int16_t error_difference = 0;

    // MOTOR VARIABLES
    static bool turn = false;
    static float power_output = 0;

  // ERROR AND TIME SEPARATION
  error = angle_reference - encoder_get_counter_value();
  time_separation = TCNT5;
  error_difference = error - prev_error;
  prev_error = error;

  // ANGLE MODULE
  error_difference = error_difference > 2048 ? error_difference - 4096: error_difference;
  error_difference = error_difference < -2048 ? 4096 - error_difference: error_difference;

  // PID
  if ((error > min_positive_angle_tolerance) || (error < min_negative_angle_tolerance)){
    power_output = proportional_gain * error + differential_gain * (error_difference) / time_separation;  
    // SATURATION & TOLERANCE
    turn = (power_output >= 0);
    power_output = power_output < 0 ? - power_output : power_output;
    power_output = power_output < min_power_output? min_power_output : power_output;
    power_output = power_output > max_power_output ? max_power_output : power_output;
  }
  else{
    turn = true;
    power_output = 0;
  }

  if(Serial.available() > 0){
    motors_stop();
    read_integers();
    Serial.print("Maximum Angle [degrees]: ");
    Serial.println(((float)(max_power_output) / (float)(proportional_gain)) * counters_to_degrees);
    Serial.print("Max Power: ");
    Serial.println(max_power_output);
    Serial.print("Minimum Angle [degrees]: ");
    Serial.println(((float)(min_power_output) / (float)(proportional_gain)) * counters_to_degrees);
    Serial.print("Min Power: ");
    Serial.println(min_power_output);
    Serial.print("Tolerance: ");
    Serial.println((float)(min_positive_angle_tolerance) * counters_to_degrees);
    Serial.println();
    Serial.print("Actual Angle [degrees]: ");
    Serial.println(error * counter_to_radians);
    Serial.print("Actual Power: ");
    Serial.println(power_output);
    Serial.println();

    static int16_t dummy;

    Serial.print("Introducir la ganancia proporcional: ");
    dummy = read_integers();
    if(dummy > 0)
    {
        Serial.println(dummy);
        proportional_gain = ((float)(dummy)) * counter_to_radians;
        Serial.println(); 
    }
    else
    {
        Serial.println((float)proportional_gain / (float)counter_to_radians);
        Serial.println(); 
    }


    Serial.print("Introducir la ganancia diferencial: ");
    dummy = read_integers();
    if(dummy > 0)
    {
        Serial.println(dummy);
        differential_gain = ((float)(dummy)) * counter_to_radians * timer_frequency / timer_prescaller_value;
        Serial.println(); 
    }
    else
    {
        Serial.println((float)(differential_gain) * timer_prescaller_value / (float)(counter_to_radians * timer_frequency));
        Serial.println(); 
    }


    Serial.print("Introducir la potencia minima: ");
    dummy = read_integers();
    if(dummy > 0)
    {
        Serial.println(dummy);
        min_power_output = dummy;  // 0.78V
        Serial.println(); 
    }
    else
    {
        Serial.println(min_power_output);
        Serial.println(); 
    }
    
    Serial.print("Introducir la tolerancia maxima: ");
    dummy = read_integers();
    if(dummy > 0)
    {
        Serial.println(dummy);
        min_positive_angle_tolerance = (dummy * (float)(degrees_to_counters));
        min_negative_angle_tolerance = - min_positive_angle_tolerance;
        Serial.println(); 
    }
    else
    {
        Serial.println((float) (min_positive_angle_tolerance) / (float)(degrees_to_counters));
        Serial.println(); 
    }

    Serial.print("Maximum Angle [degrees]: ");
    Serial.println(((float)(max_power_output) / (float)(proportional_gain)) * counters_to_degrees);
    Serial.print("Max Power: ");
    Serial.println(max_power_output);
    Serial.print("Minimum Angle [degrees]: ");
    Serial.println(((float)(min_power_output) / (float)(proportional_gain)) * counters_to_degrees);
    Serial.print("Min Power: ");
    Serial.println(min_power_output);
    Serial.print("Tolerance: ");
    Serial.println((float)(min_positive_angle_tolerance) * counters_to_degrees);
    Serial.println();
    Serial.print("Actual Angle [degrees]: ");
    Serial.println(error * counter_to_radians);
    Serial.print("Actual Power: ");
    Serial.println(power_output);
    Serial.println();  
    
    Serial.println("Envia un numero para cambiar las constantes");
    TCNT5 = 0;
  }
  

  // MOTOR MOVE
  motor_move(turn, (uint8_t)(power_output)); 


//  Serial.print("Derivada: ");
//  Serial.println((error - prev_error) / time_separation);
//  if (prev_error != error){
//    Serial.print("Angulo de Error Ticks: ");
//    Serial.println(error);
//    Serial.print("Angulo de Error Grados: ");
//    Serial.println(error * 180.0 / 2048.0);
//    Serial.print("Power: ");
//    Serial.println(power_output);
//  }
  


  
  
  
//  // DIFFERENTIATION
//  Serial.print("Nuevo valor para agregar a la derivada: ");
//  int16_t new_value = read_integers();
//  Serial.println(new_value);
//  Serial.println("El valor de la derivada con la nueva entrada:");
//  Serial.println(differential_new_value(new_value, 1.0, error_differentiation));
//  Serial.println("Los valores de los puntos de la derivada actual");
//  Serial.print("[");
//  Serial.print(error_differentiation.actual_taylor_elements[0]);
//  for(uint8_t k = 1; k < error_differentiation.differential_degree; ++k){
//   Serial.print(", ");
//   Serial.print(error_differentiation.actual_taylor_elements[k]);
//  }
//  Serial.println("]");
//  
//  Serial.println("Los valores de los puntos de la derivada anterior");
//  Serial.print("[");
//  Serial.print(error_differentiation.previous_taylor_elements[0]);
//  for(uint8_t k = 1; k < error_differentiation.differential_degree; ++k){
//   Serial.print(", ");
//   Serial.print(error_differentiation.previous_taylor_elements[k]);
//  }
//  Serial.println("]");

  
//  // INTEGRATION
//  Serial.print("Nuevo valor para agregar a la integral: ");
//  int16_t new_value = read_integers();
//  Serial.println(new_value);
//  Serial.println("El valor de la integral con la nueva entrada:");
//  Serial.println(integral_new_value(new_value, 1.0, error_integral));
//  Serial.println("Los valores de los puntos de la interal");
//  Serial.print("[");
//  Serial.print(error_integral.integral_differential_value[0]);
//  for(uint8_t k = 1; k < error_integral.integral_points; ++k){
//   Serial.print(", ");
//   Serial.print(error_integral.integral_differential_value[k]);
//  }
//  Serial.println("]");
  


//// SAMPLING
//  Serial.println("Clear Console, and send a 1");
//  while(Serial.available() <= 0){
//    delay(100);
//  }
//  Serial.read();
//  Serial.flush();
//  delay(100);
//  generate_quanser_samples();

// MEDICION ENCODER
//   static int16_t encoder_position = 0;
//    if (encoder_get_counter_value() != encoder_position){
//      encoder_position = encoder_get_counter_value();
//      Serial.println(encoder_position);
//    }

////Serial.println(analogRead(potentiometer_pin));
//// MEDICION POTENCIOMETRO
//   static int16_t encoder_position = 0;
//    if (encoder_get_counter_value() != encoder_position){
//      encoder_position = encoder_get_counter_value();
//      Serial.println(analogRead(potentiometer_pin));
//    }


}


int16_t read_integers(){
  while(Serial.available() <= 0){
    delay(100);
  }
  int16_t result = 0;
  char dato = 0;
  int16_t sign  = 1;
  while(Serial.available() > 0){
    dato = Serial.read();
    if (dato == '-'){
      sign *= -1;
    }
    else{
      result *= 10;
      result += dato - '0';
    }
  }
  result *= sign;
  return result;
}

bool timer_setup(bool active_noise_canceler, uint8_t prescaller_value){
   // MASK INTERRUPTS
  TIMSK5 = 0;
  // TIME COUNTER REGESTER
  TCCR5A = 0;
  TCCR5B = active_noise_canceler ? 0b11000000 : 0b00000000;
  OCR5A = (uint16_t)(-1);

  uint16_t prescaller_values[6] = {0,1,8,64,256,1024};

  for (uint8_t k = 0; k < 6; ++k){
    if (prescaller_value == prescaller_values[k]){
      TCCR5B |= k;
    }
  }
  TCNT5 = 0;
  return true;
}





// CALIBRATION OF ZERO
bool setup_system_to_zero_position(){
  motor_move(clockwise, 255);
  delay(100);
  //Position must be equal to start_position and motors should be off
  //START CONDITIONS
  motors_stop();
  delay(100);
  uint16_t angle = analogRead(potentiometer_pin);
  uint16_t degrees_max_angle = 45; //sets maximum potentiometer degrees values for motor power
  int16_t max_angle = potenciometer_0_degrees_value + (degrees_max_angle / 90)* (potenciometer_positive_90_degrees_value - potenciometer_0_degrees_value);
  int16_t min_angle = potenciometer_0_degrees_value - (degrees_max_angle / 90)* (potenciometer_0_degrees_value - potenciometer_negative_90_degrees_value);
  uint8_t max_power = 255;  // 5 v
  uint8_t min_power = 75; // 0.25 v
  uint8_t power = 0;
  uint16_t time_in_millis = 0;
  motors_stop();
  delay(100);

  while (angle != potenciometer_0_degrees_value){
    angle = analogRead(potentiometer_pin);
//    Serial.print("Angulo actual: ");
//    Serial.println(angle);
//    Serial.print("Angulo de referencia: ");
//    Serial.println(potenciometer_0_degrees_value);
    
    if (angle > potenciometer_0_degrees_value){
      power = (angle >= max_angle) ? max_power : max_power * (angle - potenciometer_0_degrees_value) / (max_angle - potenciometer_0_degrees_value);
      power = (power <= min_power) ? min_power : power;
//      Serial.print("Energía del motor counterclockwise: ");
//      Serial.println(power);
      motor_move(counterclockwise, power);
      while (angle > potenciometer_0_degrees_value){
        angle = analogRead(potentiometer_pin);    
      }
    }
    else if(angle < potenciometer_0_degrees_value){
      power = (angle <= min_angle) ? max_power : max_power * (potenciometer_0_degrees_value - angle) / (potenciometer_0_degrees_value - min_angle);
      power = (power <= min_power) ? min_power : power;
//      Serial.print("Energía del motor clockwise: ");
//      Serial.println(power);
      motor_move(clockwise, power);
      while (angle < potenciometer_0_degrees_value){
        angle = analogRead(potentiometer_pin);
//        Serial.println(angle);
      }
    }
    delay(1);
    motors_stop();
//    Serial.println("Motores Apagados");
    angle = analogRead(potentiometer_pin);
    delay (100);
    while (angle != analogRead(potentiometer_pin)){
      angle = analogRead(potentiometer_pin);
      delay (100);
//      Serial.println(angle);
    }
    delay(100);
    angle = analogRead(potentiometer_pin);
    delay(100);
  }
  delay(1000);
  return true;
}
