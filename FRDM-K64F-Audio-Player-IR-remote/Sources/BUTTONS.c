/*
 * BUTTONS.c
 *
 *  Created on: 26/9/2016
 *      Author: tomh6
 */

#include "BUTTONS.h"
#include "PORT.h"
#include "GPIO.h"
#include "SysTick.h"

///GPIO_MemMapPtr puertos[5]=GPIO_BASE_PTRS;
//puertos[5]->PDDR=0;

#define PIN_PUSH_BUTTON  4	//Switch3 PTA4
#define PORT_PUSH_BUTTON PORTA
#define GPIO_PORT_PUSH_BUTTON PTA

#define PIN_SW2 6 //PTC6
#define PORT_SW2 PORTC
#define GPIO_PORT_SW2 PTC

#define false 0
#define true 1




// Associate each pin to the corresponding port and gpio port using the following definitions

typedef struct{

	uint32_t pin;
	GPIO_MemMapPtr gpio_port;   // GPIO Peripheral Access Layer
	PORT_MemMapPtr port;		// PORT Peripheral Access Layer

}PIN_PORT;


// PIN - PORT


//Pushbtn
PIN_PORT pushbtn = {PIN_PUSH_BUTTON, GPIO_PORT_PUSH_BUTTON, PORT_PUSH_BUTTON};
PIN_PORT pushbtn_SW2 = {PIN_SW2, GPIO_PORT_SW2, PORT_SW2};

static bool *buttons_activity[3][2]; //tres actividades, 2 botones CUIDADO! para cargar los valores se tiene que usar button:id -2 porque tenemos sw2 y sw3.
static bool button2_state = false;
static bool button2_RisingEdge = false;
static bool button2_FallingEdge = false;
static bool button3_state = false;
static bool button3_RisingEdge = false;
static bool button3_FallingEdge = false;

static bool button2_Prevstate = false;
static bool button3_Prevstate = false;

void BUTTONS_Init()
{
	PCRstr UserPCR;
	PORT_Init();
	GPIO_Init();
    SysTick_Init();
	registerCallback(&BUTTONS_PISR, 1);
	/******** PUSH BUTTON PIN***********/
			UserPCR.PCR=false;			// Default All false, Set only those needed

			UserPCR.FIELD.DSE=true;
			UserPCR.FIELD.MUX=PORT_mGPIO;
			UserPCR.FIELD.IRQC=PORT_eDisabled;	// EJ1
			UserPCR.FIELD.PE = true; // habilito el Pull Down/Up del pulsador.
			UserPCR.FIELD.PS = true; // true = pullUp, false = pullDown.

			PORT_Configure2 (pushbtn.port, pushbtn.pin, UserPCR);
			PORT_Configure2 (pushbtn_SW2.port, pushbtn_SW2.pin, UserPCR);

			GPIO_SetDirection(pushbtn.gpio_port ,pushbtn.pin, GPIO__IN);
			GPIO_SetDirection(pushbtn_SW2.gpio_port ,pushbtn_SW2.pin, GPIO__IN);

			buttons_activity[STATE][0] = &button2_state;
			buttons_activity[RISING_EDGE][0] = &button2_RisingEdge;
			buttons_activity[FALLING_EDGE][0] = &button2_FallingEdge;
			buttons_activity[STATE][1] = &button3_state;
			buttons_activity[RISING_EDGE][1] = &button3_RisingEdge;
			buttons_activity[FALLING_EDGE][1] = &button3_FallingEdge;


}

void BUTTONS_PISR()
{
	get_button(BUTTON_2);
	if((button2_state == true) && (button2_Prevstate == false)) // 1->0
	{
		button2_FallingEdge = true;
	}
	else if((button2_state == false) && (button2_Prevstate == true)) // 0->1
	{
		button2_RisingEdge = true;
	}
	get_button(BUTTON_3);
	if((button3_state == true) && (button3_Prevstate == false)) // 1->0
		{
			button3_FallingEdge = true;
		}
		else if((button3_state == false) && (button3_Prevstate == true)) // 0->1
		{
			button3_RisingEdge = true;
		}
	button2_Prevstate = button2_state;
	button3_Prevstate = button3_state;
}

bool BUTTONS_Service(unsigned int service, unsigned int button_id)
{
	bool ret = *buttons_activity[service][button_id -2];
	*buttons_activity[service][button_id -2] = false;
	return ret;
}

static void get_button (unsigned int button_id){
    GPIOData_t GPIOData_aux=0;
    switch (button_id){
        case BUTTON_2:{
            GPIOData_aux=GPIO_Read(pushbtn_SW2.gpio_port,pushbtn_SW2.pin);
                if(GPIOData_aux!=0){
                    button2_state=false;
                }
                else{
                    button2_state=true;
                }
        }break;

        case BUTTON_3:{
            GPIOData_aux=GPIO_Read(pushbtn.gpio_port,pushbtn.pin);
                if(GPIOData_aux!=0){
                    button3_state=false;
                }
                else{
                    button3_state=true;
                }
        }break;
        default:{
            ///ERROR CASE!
        }
    }
}
