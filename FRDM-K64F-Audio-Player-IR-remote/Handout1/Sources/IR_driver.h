/*
 * IR_driver.h
 *
 *  Created on: 3/7/2018
 *      Author: Tomas & Dexter
 */

#ifndef SOURCES_IR_DRIVER_H_
#define SOURCES_IR_DRIVER_H_

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

bool infrared_setup();




#endif /* SOURCES_IR_DRIVER_H_ */
