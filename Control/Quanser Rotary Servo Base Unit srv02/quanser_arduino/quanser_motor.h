#ifndef __QUANSER_MOTOR_H_INCLUDED__
#define __QUANSER_MOTOR_H_INCLUDED__
//EXTERNAL LIBRARIES
// #include <Stdint.h>
#include <Arduino.h>

/*MOVE MOTORS*/
bool motor_move(bool move_clockwise);

/*STOP MOTORS*/
bool motors_stop();

/*SETUP MOTOR PINS*/
bool motor_set_pins(uint8_t clockwise_pin, uint8_t counterclockwise_pin);

#endif
