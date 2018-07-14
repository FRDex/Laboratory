
#ifndef SOURCES_SYSTICK_H_
#define SOURCES_SYSTICK_H_

#include "Template/LDM.h"

//__ISR__       SysTick_Handler (void);

void 	 	  SysTick_Init 	  (void);

void registerCallback(void (*fun)());
uint16_t getSysFreq(void);
void setSysFreq (uint16_t freq);


#endif

//FALTA funcion que sea registrar CALLBACK que sea llamada desde la inicialización de LED y me de la direccion de la función a la que quiero llamar.
