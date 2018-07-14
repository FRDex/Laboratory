#include <Stdint.h>
#include "quadrature_encoder.h"
#include "quanser_motor.h"
#include "potentiometer.h"
//#include "quanser_samples.h"

#include "differentiation.h"
#include "integral.h"

static uint8_t encoder_clockwise_pin = 2, encoder_counterclockwise_pin = 3;
static uint8_t motor_clockwise_pin = 6, motor_counterclockwise_pin = 5;
static uint8_t potentiometer_pin = A0;

bool setup_system_to_zero_position();



int16_t read_integers();


// PID
#define INTEGRAL_POINTS 5
#define DIFFERENTIAL_POINTS 2 // MIN 1 MAX 4

static uint8_t error_integral_points = INTEGRAL_POINTS;
static float error_integral_array [INTEGRAL_POINTS];
static integral_struct error_integral;

static uint8_t error_differentiation_points = DIFFERENTIAL_POINTS;
static differentiation_struct error_differentiation;



static const float counter_to_radians = PI / 2048.0;
static const float degrees_to_radians = PI / 180.0;
static const int16_t angle_reference = 180.0 * degrees_to_radians / counter_to_radians;

static int16_t error = 0;
static int16_t prev_error = 0;
static int16_t error_difference = 0;
static float power_output = 0;

static const float proportional_gain = 2500.0 * counter_to_radians;
static const float differential_gain = 100.0 * counter_to_radians / 10e6;

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
  
  Serial.print("Activation of Encoder is complete? ");
  Serial.println(setup_conditions[encoder_activate()]);
  

//  Serial.print("Setup Quanser Samples is complete? ");
//  Serial.println(setup_conditions[setup_quanser_samples()]);


  // INTEGRATION
//  Serial.print("Integration Setup is complete? ");
//  Serial.println(setup_conditions[integral_setup(error_integral_points, error_integral, error_integral_array)]);
//
//  Serial.print("Differentiation Setup is complete? ");
//  Serial.println(setup_conditions[differential_setup(error_differentiation_points, error_differentiation)]);
  
}

// CALIBRATION OF ZERO
bool setup_system_to_zero_position(){
  motor_move(clockwise, 255);
  delay(100);
  //Position must be equal to start_position and motors should be off
  //START CONDITIONS
  motors_stop();
  delay(100);
  bool starting_angle_turn = false;
  bool new_angle_turn = false;
  float angle = potentiometer_get_angle();
  float abs_angle = angle > 0 ? angle : -angle;
  float angle_max_power = PI;
  uint8_t max_power = 255;  // 5 v
  uint8_t min_power = 75; // 0.25 v
  uint8_t power = 0;
  uint16_t time_in_millis = 0;
  motors_stop();
  delay(100);

  while (angle != 0){
    angle = potentiometer_get_angle();
    starting_angle_turn = angle < 0;
    new_angle_turn = starting_angle_turn;

    abs_angle = starting_angle_turn ? angle : -angle;
    power = (abs_angle >= angle_max_power) ? max_power : (float)(max_power) * (abs_angle / angle_max_power);
    power = power >= min_power ? power : min_power;

    motor_move(starting_angle_turn, power);
    while ((starting_angle_turn == new_angle_turn) && (angle != 0)){
      angle = potentiometer_get_angle();
      new_angle_turn = angle < 0;
    }
    
    motors_stop();
    angle = potentiometer_get_angle();
    delay (100);
    while (angle != potentiometer_get_angle()){
      angle = potentiometer_get_angle();
      delay (100);
    }
    delay(100);
    angle = potentiometer_get_angle();
  }
  delay(1000);
  return true;
}



void loop() {
  
  static uint16_t actual_time = 0, prev_time = 0, time_separation = 0;
  static bool turn = false;
//  Serial.println(encoder_get_counter_value());
  actual_time = micros();
  time_separation = actual_time - prev_time;
  prev_time = actual_time;
  error_difference = (error - prev_error) % 2048;
  error = -encoder_get_counter_value(); //angle reference is zero angle_reference - encoder_get_counter_value();
  power_output = proportional_gain * error + differential_gain * (error_difference) / time_separation;
  prev_error = error;
  
  
  if (power_output > 0){
    turn = clockwise;
  }else{
    turn = counterclockwise;
    power_output = - power_output;
  }
  
  
    
  if(power_output < 15){
    motors_stop();
  }
  else{
  power_output = power_output > 255 ? 255 : power_output;
  motor_move(turn, (uint8_t)(power_output));  
  }
 
//  Serial.print("Derivada: ");
//  Serial.println((error - prev_error) / time_separation);
//  Serial.print("Error: ");
//  Serial.println(error);
//  Serial.print("Power: ");
//  Serial.println(power_output);
  
  
  
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

//// MEDICION ENCODER
//   static int16_t encoder_position = 0;
//    if (encoder_get_counter_value() != encoder_position){
//      encoder_position = encoder_get_counter_value();
//      Serial.println(encoder_position);
//    }
//Serial.println(analogRead(potentiometer_pin));
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
