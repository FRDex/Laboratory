#ifndef __quadrature_encoder_H_INCLUDED__
#define __quadrature_encoder_H_INCLUDED__

/*POSITION DATA RELATED*/

//RETURN THE ENCODER VALUE
inline int16_t get_encoder_value();
//SET THE ENCODER VALUE OF REFERENCE
bool set_encoder_reference_value(int16_t value);
//SET ZERO REFERENCE FOR THE ENCODER
bool set_encoder_zero_reference();
//RETURN TRUE IF IT IS CLOCKWISE
bool get_turn_status();


/*ENCODER SETUP*/

// RUN set_encoder_pins,set_encoder_limit AND active_encoder IN ORDER TO SET ACTIVE THE ENCODER
bool set_encoder(uint8_t clockwise_pin, uint8_t counterclockwise_pin, int16_t max_value);
// SET PINS AS INPUTS AND ATTACH INTTERUPTS
bool active_encoder();
// DETACH INTERRUPTS, RETURN TRUE IF THE ENCODER WAS ACTIVE AND COULD BE DEACTIVATED
bool deactivate_encoder();

#endif
