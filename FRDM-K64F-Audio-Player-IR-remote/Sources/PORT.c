
#include "PORT.h"

static unsigned int wasInit = 0;
void PORT_Init (void)
{
	if(!wasInit)
	{
		SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
		SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
		SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
		SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
		SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;

		PORT_ClearPortFlags (PORTA);
		PORT_ClearPortFlags (PORTB);
		PORT_ClearPortFlags (PORTC);
		PORT_ClearPortFlags (PORTD);
		PORT_ClearPortFlags (PORTE);

		NVIC_EnableIRQ(PORTA_IRQn);
		NVIC_EnableIRQ(PORTB_IRQn);
		NVIC_EnableIRQ(PORTC_IRQn);
		NVIC_EnableIRQ(PORTD_IRQn);
		NVIC_EnableIRQ(PORTE_IRQn);
		wasInit = 1;

		/*
		 * TO DO
		 */
	}

}

void PORT_SetPullResistorMode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_PE_MASK;
	port->PCR[bit] |= PORT_PCR_PE(mode);
}

void PORT_SetPullUpMode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_PS_MASK;
	port->PCR[bit] |= PORT_PCR_PS(mode);
}

void PORT_SetOpenDrainMode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_ODE_MASK;
	port->PCR[bit] |= PORT_PCR_ODE(mode);
}

void PORT_SetPassiveFilterMode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_PFE_MASK;
	port->PCR[bit] |= PORT_PCR_PFE(mode);
}

void PORT_SetSlowSlewRateMode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_SRE_MASK;
	port->PCR[bit] |= PORT_PCR_SRE(mode);
}

void PORT_SetHighDriveStrengthMode (PORT_t port, PORTBit_t bit, bool mode)
{
	port->PCR[bit] &= ~PORT_PCR_DSE_MASK;
	port->PCR[bit] |= PORT_PCR_DSE(mode);
}

void PORT_SetMuxMode (PORT_t port, PORTBit_t bit, PORTMux_t mux)
{
	port->PCR[bit] &= ~PORT_PCR_MUX_MASK;
	port->PCR[bit] |= PORT_PCR_MUX(mux);
}

void PORT_SetInterruptMode (PORT_t port, PORTBit_t bit, PORTEvent_t event)
{
	port->PCR[bit] &= ~PORT_PCR_IRQC_MASK;
	port->PCR[bit] |= PORT_PCR_IRQC(event);
}

bool PORT_IsInterruptPending (PORT_t port, PORTBit_t bit)
{
	return port->PCR[bit] & PORT_PCR_ISF_MASK;
}

void PORT_ClearInterruptFlag (PORT_t port, PORTBit_t bit)
{
	port->PCR[bit] |= PORT_PCR_ISF_MASK;
}

void PORT_Configure (PORT_t port, PORTBit_t bit, bool pr, bool pu, bool od, bool pf, bool ssr, bool hds, PORTMux_t mux, PORTEvent_t event)
{



	port->PCR[bit] = PORT_PCR_PE(pr) |
			         PORT_PCR_PS(pu) |
					 PORT_PCR_ODE(od) |
					 PORT_PCR_PFE(pf) |
					 PORT_PCR_SRE(ssr) |
					 PORT_PCR_DSE(hds) |
					 PORT_PCR_MUX(mux) |
					 PORT_PCR_IRQC(event) |
					 PORT_PCR_ISF_MASK;


}


// This is a to do function see led.c
void PORT_Configure2 (PORT_t port, PORTBit_t bit,PCRstr UserPCR)
{

	port->PCR[bit]=UserPCR.PCR ;

}


PORTFlags_t PORT_GetPortFlags (PORT_t port)
{
	return port->ISFR & PORT_ISFR_ISF_MASK;
}

void PORT_ClearPortFlags (PORT_t port)
{
	port->ISFR = PORT_ISFR_ISF_MASK;
}

uint32_t speed = 4;
static unsigned int IRQ_count = 0;
__ISR__	PORTA_IRQHandler			  (void)
{
	bool portA_ext = PORT_IsInterruptPending(PORTA, 1);
	bool portA_sw3 = PORT_IsInterruptPending(PORTA, 4);
	if (portA_ext)
	{
		//Hacer función correspondiente a este PIN
		PORT_ClearInterruptFlag (PORTA, 1);
	}
	else if (portA_sw3)
	{
		//Hacer función correspondiente a este PIN
		PORT_ClearInterruptFlag(PORTA, 4);
	}
	++IRQ_count;
	setSpeed();

	PORT_ClearPortFlags (PORTA);
}

__ISR__ 	PORTC_IRQHandler			  (void)
{
	bool portC_SW2 = PORT_IsInterruptPending(PORTC, 6);
	if(portC_SW2)
	{
		//Hacer función correspondiente a este PIN
		PORT_ClearInterruptFlag(PORTC, 6);
	}
	++IRQ_count; //Se me inicializan las flags arriba ¿por qué?
	setSpeed();

	PORT_ClearPortFlags (PORTC);
}

void setSpeed()
{
	if (IRQ_count == 1)
	{
		speed = 16;
	} else
	{
		speed = 2;
		IRQ_count = 0;
	}
}
uint32_t getSpeed()
{
	return speed;
}



