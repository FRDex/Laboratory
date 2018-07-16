
#ifndef SOURCES_SYSTICK_H_
#define SOURCES_SYSTICK_H_

#include "Template/LDM.h"

#define TOPCALLBACKS 10

typedef void (*systickCalback)(void);

void	SystickRegisterCallback		(systickCalback Systick_callback_input,uint32_t preesc);
void 	SysTick_Init 	  			(void);
void Set_Systick_Frequency(unsigned int frequency);
#endif
