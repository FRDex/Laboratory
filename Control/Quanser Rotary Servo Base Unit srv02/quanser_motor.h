#ifndef __quadrature_encoder_H_INCLUDED__
#define __quadrature_encoder_H_INCLUDED__

/*MOVE MOTORS*/
inline void motor_move(bool move_clockwise);
/*STOP MOTORS*/
inline void motors_stop();

/*SETUP MOTOR PINS*/
bool motor_set_pins(uint8_t clockwise_pin, uint8_t counterclockwise_pin);
#endif
