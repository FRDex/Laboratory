/*
 * DAC.h
 *
 *  Created on: 9/7/2018
 *      Author: tomh6
 */

#ifndef SOURCES_DAC_H_
#define SOURCES_DAC_H_

#include "Template/LDM.h"

typedef DAC_Type *DAC_t;
typedef uint16_t DACData_t;

void DAC_Init	 (void);

void DAC_SetData (DAC_t, DACData_t);

void DAC_PISR (void);

void DAC_play(uint16_t sample);


#endif /* SOURCES_DAC_H_ */
