#include "quanser_motor.h"
/*HARDWARE INPUT*/
static uint8_t motor_clockwise_pin, motor_counterclockwise_pin;

/*CONDITIONS*/
static bool motor_is_set = false;

// FUNCTIONS DEFINITIONS

//HEADER
static bool motor_move(uint8_t active_motor, uint8_t pasive_motor, uint8_t power);

/*MOTOR MOVE*/
bool motor_move(bool move_clockwise, uint8_t power){
    if (move_clockwise){
      motor_move(motor_clockwise_pin, motor_counterclockwise_pin, power);
    }
    else{
      motor_move(motor_counterclockwise_pin, motor_clockwise_pin, power);
    }
}

static bool motor_move(uint8_t active_motor, uint8_t pasive_motor, uint8_t power){
  if (motor_is_set) {
    analogWrite(pasive_motor, 0);
    analogWrite(active_motor, power);
    return true;
  }
  return false;
}

/*MOTOR MOVE*/
bool motors_stop(){
  if (motor_is_set) {
    analogWrite(motor_clockwise_pin, 0);
    analogWrite(motor_counterclockwise_pin, 0);
  }
  return true;
}

/*MOTOR SETUP*/
bool motor_set_pins(uint8_t clockwise_pin, uint8_t counterclockwise_pin){
  if(clockwise_pin != counterclockwise_pin){
      motor_clockwise_pin = clockwise_pin;
      motor_counterclockwise_pin = counterclockwise_pin;
      pinMode(motor_clockwise_pin, OUTPUT);
      pinMode(motor_counterclockwise_pin, OUTPUT);
      motor_is_set = true;
      return true;
  }
  return false;
}
