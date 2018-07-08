#include <Stdint.h>
#include "quadrature_encoder.h"
#include "quanser_motor.h"
#include "potentiometer.h"

#include "quanser_samples.h"
#include "differentiation.h"
#include "integral.h"

static uint8_t encoder_clockwise_pin = 2, encoder_counterclockwise_pin = 3;
static uint8_t motor_clockwise_pin = 6, motor_counterclockwise_pin = 5;
static uint8_t potentiometer_pin = A0;

bool setup_system_to_zero_position();

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

  Serial.print("Activation of Encoder is complete? ");
  Serial.println(setup_conditions[encoder_activate()]);

  Serial.print("Setup Quanser Samples is complete? ");
  Serial.println(setup_conditions[setup_quanser_samples(false)]);
}

// CALIBRATION OF ZERO
bool setup_system_to_zero_position(){
  motor_move(clockwise, 255);
  delay(100);
  //Position must be equal to start_position and motors should be off
  //START CONDITIONS
  motors_stop();
  delay(100);
  float angle = potentiometer_get_angle();
  float abs_angle = angle > 0 ? angle : -angle;
  float angle_max_power = PI / 2.0;
  uint8_t max_power = 255;  // 5 v
  uint8_t min_power = 13; // 0.25 v
  uint8_t power = 0;
  
  while (angle != 0){
    motor_move(angle < 0, power);
    angle = potentiometer_get_angle();
    abs_angle = angle > 0 ? angle : -angle;
    power = (abs_angle >= angle_max_power) ? max_power : (float)(max_power) * (abs_angle / angle_max_power);
    power = power >= min_power ? power : min_power;
    delay(1);
    motors_stop();
    delay(100);
  }
  delay(1000);
  return true;
}


void loop() {
// SAMPLING
  Serial.println("Clear Console, and send a 1");
  while(Serial.available() <= 0){
    delay(100);
  }
  Serial.read();
  Serial.flush();
  delay(100);
  generate_quanser_samples();

//// MEDICION ENCODER  
//   static int16_t encoder_position = 0;
//    if (encoder_get_counter_value() != encoder_position){
//      encoder_position = encoder_get_counter_value();
//      Serial.println(encoder_position);
//    }

//// MEDICION POTENCIOMETRO  
//   static int16_t encoder_position = 0;
//    if (encoder_get_counter_value() != encoder_position){
//      encoder_position = encoder_get_counter_value();
//      Serial.println(analogRead(potentiometer_pin));
//    }


}
