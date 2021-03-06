
#include "SysTick.h"
#include <stdio.h>

static systickCalback  systick_callbacks[TOPCALLBACKS];
static uint32_t 			callback_preescaler[TOPCALLBACKS];
static uint8_t totaCallbacks = 0;

void DSP_PISR (void);
void (*Systick_callback)(void);

__ISR__ SysTick_Handler (void)
{
	static uint32_t preescaler = 0;
	static uint8_t k=0;
	for (k=0; k < totaCallbacks; ++k){
		if(!(preescaler % callback_preescaler[k])){
		systick_callbacks[k]();}
	}
	++preescaler;
}

void	SystickRegisterCallback		(systickCalback Systick_callback_input, uint32_t preesc){
	callback_preescaler[totaCallbacks] = preesc;
	systick_callbacks[totaCallbacks] = Systick_callback_input;
	++totaCallbacks;
}

void SysTick_Init (void)
{
	SysTick->CTRL = 0x00;
	SysTick->LOAD = 2267;
	SysTick->VAL  = 0x00;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

void Set_Systick_Frequency(unsigned int frequency){
	SysTick->LOAD = (100000000/(frequency))-1; // 26103 - 3kHz
}
