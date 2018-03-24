#ifndef __quadrature_encoder_H_INCLUDED__
#define __quadrature_encoder_H_INCLUDED__

/*POSITION DATA RELATED*/
inline int16_t encoder_get_value();
bool encoder_is_turning_clockwise();

/*ENCODER SETUP*/
bool encoder_set(uint8_t clockwise_pin, uint8_t counterclockwise_pin, uint16_t max_value);
bool encoder_set(uint8_t clockwise_pin, uint8_t counterclockwise_pin);  // ASSUMES DEFAULT VALUE (4096)
bool encoder_set_zero_reference();  // SET THE ACTUAL MOTOR POSITION AS THE ZERO REFERENCE FOR THE ENCODER
bool encoder_set_reference_value(int16_t value); // SET AN OFFSET FROM THE ACTUAL MOTOR POSITION AS THE ZERO REFERENCE FOR THE ENCODER
bool encoder_activate();  // SET PINS AS INPUTS AND ATTACH INTTERUPTS. (MUST SET ENCODER FIRST)
bool encoder_deactivate();  // DETACH INTERRUPTS

#endif
