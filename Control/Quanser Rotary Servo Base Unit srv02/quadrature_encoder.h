#ifndef __QUADRATURE_ENCODER_H_INCLUDED__
#define __QUADRATURE_ENCODER_H_INCLUDED__

/*POSITION DATA RELATED*/
float encoder_get_value();
uint16_t encoder_get_counter_value();
bool encoder_is_turning_clockwise();
bool encoder_is_radians();

/*ENCODER SETUP*/
bool encoder_set(uint8_t clockwise_pin, uint8_t counterclockwise_pin, uint16_t max_value);
bool encoder_set(uint8_t clockwise_pin, uint8_t counterclockwise_pin);  // ASSUMES DEFAULT VALUE (4096)
bool encoder_set_zero_reference();  // SET THE ACTUAL MOTOR POSITION AS THE ZERO REFERENCE FOR THE ENCODER
bool encoder_set_reference_value(float value); // SET AN OFFSET FROM THE ACTUAL MOTOR POSITION AS THE ZERO REFERENCE FOR THE ENCODER
bool encoder_activate();  // SET PINS AS INPUTS AND ATTACH INTTERUPTS. (MUST SET ENCODER FIRST)
bool encoder_deactivate();  // DETACH INTERRUPTS
bool encoder_set_radians(bool radians);

#endif
