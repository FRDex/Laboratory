#include <quanser_motor.h>
bool motor_is_set = false;
uint8_t motor_clockwise_pin, motor_counterclockwise_pin;
//HEADER
inline void motor_move(uint8_t active_motor, uint8_t pasive_motor);

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

inline void motor_move(uint8_t active_motor, uint8_t pasive_motor){
  if (motor_is_set) {
    digitalWrite(pasive_motor, LOW);
    digitalWrite(active_motor, HIGH);
  }
}

inline void motor_move(bool move_clockwise){
  if (motor_is_set) {
    if (move_clockwise){
      motor_move(motor_clockwise_pin, motor_counterclockwise_pin);
    }
    else{
      motor_move(motor_counterclockwise_pin, motor_clockwise_pin);
    }
  }
}

inline void motors_stop(){
  if (motor_is_set) {
    digitalWrite(motor_clockwise_pin, LOW);
    digitalWrite(motor_counterclockwise_pin, LOW);
  }
}
