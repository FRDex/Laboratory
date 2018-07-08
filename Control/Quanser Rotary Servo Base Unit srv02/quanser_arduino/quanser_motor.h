#ifndef __QUANSER_MOTOR_H_INCLUDED__
#define __QUANSER_MOTOR_H_INCLUDED__
//EXTERNAL LIBRARIES
// #include <Stdint.h>
#include <Arduino.h>

#define clockwise true
#define counterclockwise false
/*MOVE MOTORS*/
bool motor_move(bool move_clockwise, uint8_t power);

/*STOP MOTORS*/
bool motors_stop();

/*SETUP MOTOR PINS*/
bool motor_set_pins(uint8_t clockwise_pin, uint8_t counterclockwise_pin);

#endif
