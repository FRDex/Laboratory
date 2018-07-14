/*
 * DAC.c
 *
 *  Created on: 9/7/2018
 *      Author: tomh6
 */

#include "DAC.h"

void DAC_play(uint16_t sample){
	DAC_SetData(DAC0, sample);
}

void DAC_Init (void)
{
	SIM_SCGC2 |= SIM_SCGC2_DAC0_MASK;
	SIM_SCGC2 |= SIM_SCGC2_DAC1_MASK;

	DAC0->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK | DAC_C0_DACTRGSEL_MASK;
	DAC1->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK | DAC_C0_DACTRGSEL_MASK;
}

void DAC_SetData (DAC_t dac, DACData_t data)
{
	dac->DAT[0].DATL = DAC_DATL_DATA0(data);
	dac->DAT[0].DATH = DAC_DATH_DATA1(data >> DAC_DATL_DATA0_WIDTH);
}

void DAC_PISR (void)
{

	static uint16_t k = 0;

	DAC_SetData(DAC0, k++);

	k %= 0x0FFF;
}




