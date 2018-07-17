#include <Stdint.h>
#include "quadrature_encoder.h"
#include "quanser_motor.h"
#include "potentiometer.h"

// PIO
static uint8_t encoder_clockwise_pin = 2, encoder_counterclockwise_pin = 3;
static uint8_t motor_clockwise_pin = 6, motor_counterclockwise_pin = 5;
static uint8_t potentiometer_pin = A0;

// MAGNITUDE CONVERSION
static const float counter_to_radians = PI / 2048.0;
static const float counters_to_degrees = 180.0 / 2048.0;
static const float degrees_to_counters = 2048.0 / 180.0;
static const float degrees_to_radians = PI / 180.0;

// STARTING ANGLE REFERENCE FROM ZERO
static int16_t angle_reference = 180.0 * degrees_to_radians / counter_to_radians;

// TIMER CONFIGURATION
static uint8_t timer_prescaller_value = 1;
static const float timer_frequency = 16e6;

// LOCAL FUNCTION HEADERS
bool setup_system_to_zero_position();
bool timer_setup(bool active_noise_canceler = false, uint8_t prescaller_value = 1);
int16_t read_integers();


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////// - VARIABLES PID - /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
// MOTOR VARIABLES
static uint8_t max_power_output = 255; // 5V
static uint8_t min_power_output = 30;  // 0.59V

// ERROR TOLERANCE
static int16_t min_positive_angle_tolerance = (1.0 * (float)(degrees_to_counters));
static int16_t min_negative_angle_tolerance = - min_positive_angle_tolerance;

// PID GAINS
static float proportional_gain = 350.0 * counter_to_radians;
static float differential_gain = 100.0 * counter_to_radians * timer_frequency / timer_prescaller_value;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////// - VARIABLES PID - /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

  Serial.println();
  Serial.println("Configuración Inicial del PID");
  
  Serial.print("La ganancia proporcional: ");
  Serial.println((float)proportional_gain / (float)counter_to_radians);
  Serial.print("La ganancia diferencial: ");
  Serial.println((float)(differential_gain) * timer_prescaller_value / (float)(counter_to_radians * timer_frequency));
  Serial.print("Potencia minima: ");
  Serial.println(min_power_output);
  Serial.print("Tolerancia maxima: ");
  Serial.println((float) (min_positive_angle_tolerance) / (float)(degrees_to_counters));
  
  Serial.print("Angulo de la maxima potencia [grados]: ");
  Serial.println(((float)(max_power_output) / (float)(proportional_gain)) * counters_to_degrees);
  Serial.print("Maxima Tension: ");
  Serial.println(max_power_output);
  Serial.print("Minimo Angulo [grados]: ");
  Serial.println(((float)(min_power_output) / (float)(proportional_gain)) * counters_to_degrees);
  Serial.print("Minima Tension: ");
  Serial.println(min_power_output);
  Serial.print("Tolerancia [grados]: ");
  Serial.println((float)(min_positive_angle_tolerance) * counters_to_degrees);
  Serial.println();
  
  Serial.println((float)(differential_gain) * timer_prescaller_value / (float)(counter_to_radians * timer_frequency));


  Serial.println("Envia un numero para cambiar los parametros");
  
}


    
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

    static int16_t dummy;

    Serial.print("Introducir la ganancia proporcional: ");
    dummy = read_integers();
    if(dummy >= 0)
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
    if(dummy >= 0)
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
    if(dummy >= 0)
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
    if(dummy >= 0)
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


    Serial.print("Angulo maximo [grados]: ");
    Serial.println(((float)(max_power_output) / (float)(proportional_gain)) * counters_to_degrees);
    Serial.print("Maxima Tension: ");
    Serial.println(max_power_output);
    Serial.print("Angulo minimo [grados]: ");
    Serial.println(((float)(min_power_output) / (float)(proportional_gain)) * counters_to_degrees);
    Serial.print("Minima Tension: ");
    Serial.println(min_power_output);
    Serial.print("Tolerancia: ");
    Serial.println((float)(min_positive_angle_tolerance) * counters_to_degrees);
    Serial.println();
    Serial.print("Angulo actual [grados]: ");
    Serial.println(error * counter_to_radians);
    Serial.print("Tension actual: ");
    Serial.println(power_output);
    Serial.println();  
    
    Serial.println("Envia un numero para cambiar los parametros");
    TCNT5 = 0;
  }
  

  // MOTOR MOVE
  motor_move(turn, (uint8_t)(power_output)); 


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
    if(dato == '.'){
      return (-1);
    }
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

    
    if (angle > potenciometer_0_degrees_value){
      power = (angle >= max_angle) ? max_power : max_power * (angle - potenciometer_0_degrees_value) / (max_angle - potenciometer_0_degrees_value);
      power = (power <= min_power) ? min_power : power;
      motor_move(counterclockwise, power);
      while (angle > potenciometer_0_degrees_value){
        angle = analogRead(potentiometer_pin);    
      }
    }
    else if(angle < potenciometer_0_degrees_value){
      power = (angle <= min_angle) ? max_power : max_power * (potenciometer_0_degrees_value - angle) / (potenciometer_0_degrees_value - min_angle);
      power = (power <= min_power) ? min_power : power;
      motor_move(clockwise, power);
      while (angle < potenciometer_0_degrees_value){
        angle = analogRead(potentiometer_pin);
      }
    }
    delay(1);
    motors_stop();
    angle = analogRead(potentiometer_pin);
    delay (100);
    while (angle != analogRead(potentiometer_pin)){
      angle = analogRead(potentiometer_pin);
      delay (100);
    }
    delay(100);
    angle = analogRead(potentiometer_pin);
    delay(100);
  }
  delay(1000);
  return true;
}
