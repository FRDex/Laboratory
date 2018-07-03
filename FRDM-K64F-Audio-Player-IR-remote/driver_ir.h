#ifndef __DRIVER_IR_H_INCLUDED__
#define __DRIVER_IR_H_INCLUDED__
//EXTERNAL LIBRARIES
#include "Template/LDM.h"
#include "PORT.h"
#include "GPIO.h"
#include "FTM.h"

/*GETTERS*/
bool infrared_is_data_ready();
uint16_t infrared_read_data();

/*INTERRUPT*/
void infrared_interrupt();

/*SETUP*/
const uint8_t infrared_port = 9;
const uint8_t infrared_prescaller = 128; // POWER OF TWO FROM 1 to 128
bool infrared_setup();

#endif
