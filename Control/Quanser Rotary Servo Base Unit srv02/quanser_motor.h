#ifndef __quadrature_encoder_H_INCLUDED__
#define __quadrature_encoder_H_INCLUDED__

/*MOVE MOTORS*/
bool motor_move(bool move_clockwise);
/*STOP MOTORS*/
bool motors_stop();

/*SETUP MOTOR PINS*/
bool motor_set_pins(uint8_t clockwise_pin, uint8_t counterclockwise_pin);

#endif
