#ifndef __QUADRATURE_ENCODER_H_INCLUDED__
#define __QUADRATURE_ENCODER_H_INCLUDED__
//EXTERNAL LIBRARIES
#include <Arduino.h>

// REFERENCES
#ifndef PI
#define PI (float)(3.14159265358979)
#endif

/*POSITION DATA RELATED*/
float encoder_get_value();
uint16_t encoder_get_counter_value();
bool encoder_is_turning_clockwise();
bool encoder_is_radians();

/*ENCODER SETUP*/
bool encoder_set(uint8_t clockwise_pin, uint8_t counterclockwise_pin, bool radians = true, uint16_t max_value = 4096);
bool encoder_set_actual_position_to_zero_reference();  // SET THE ACTUAL MOTOR POSITION AS THE ZERO REFERENCE FOR THE ENCODER
bool encoder_set_reference_value(float value); // SET AN OFFSET FROM THE ACTUAL MOTOR POSITION AS THE ZERO REFERENCE FOR THE ENCODER
bool encoder_activate();  // SET PINS AS INPUTS AND ATTACH INTTERUPTS. (MUST SET ENCODER FIRST)
bool encoder_deactivate();  // DETACH INTERRUPTS
bool encoder_set_radians(bool radians = true);

#endif
