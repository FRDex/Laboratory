
#include "LED.h"
#include "PORT.h"
#include "GPIO.h"
#include "SysTick.h"

///GPIO_MemMapPtr puertos[5]=GPIO_BASE_PTRS;
//puertos[5]->PDDR=0;


#define PIN_RED 	22   //PTB22
#define PIN_BLUE 	21   //PTB21
#define PIN_GREEN 	26 	 //PTE26

#define false 0
#define true 1




// Associate each pin to the corresponding port and gpio port using the following definitions

typedef struct{

	uint32_t pin;
	GPIO_MemMapPtr gpio_port;   // GPIO Peripheral Access Layer
	PORT_MemMapPtr port;		// PORT Peripheral Access Layer

}PIN_PORT;

typedef struct {

	PIN_PORT red;
	PIN_PORT blue;
	PIN_PORT green;

}LED_RGB;

// PIN - PORT
LED_RGB ledrgb ={

		{PIN_RED,PTB,PORTB},	// Red pin is on PORTB
		{PIN_BLUE,PTB,PORTB},   // Blue pin is on PORTB
		{PIN_GREEN,PTE,PORTE}   // Green pin is on PORTE
};

#define LEDRGB_GPIO_PORT(x) ledrgb.x.gpio_port
#define LEDRGB_PORT(x) ledrgb.x.port
#define LEDRGB_PIN(x) ledrgb.x.pin



// Define Led Polarity

#define RED_ON 		GPIO__LO  // Active low
#define BLUE_ON 	GPIO__LO
#define GREEN_ON 	GPIO__LO

#define RED_OFF		GPIO__HI  // Active low
#define BLUE_OFF 	GPIO__HI
#define GREEN_OFF 	GPIO__HI


typedef struct {
uint32_t red;
uint32_t blue;
uint32_t green;
}LEDColor_t;

LEDColor_t Led_States[]= {

		{RED_ON,BLUE_ON, GREEN_ON},     // White
		{RED_ON,BLUE_OFF,GREEN_OFF},	// Red
		{RED_OFF,BLUE_ON,GREEN_OFF},	// Blue
		{RED_OFF,BLUE_OFF,GREEN_ON},	// Green
		{RED_OFF,BLUE_ON,GREEN_ON},		// Cyan (Blue+Green)
		{RED_ON,BLUE_ON,GREEN_OFF},		// Magenta (Red +Blue)
		{RED_ON,BLUE_OFF,GREEN_ON},		// Yellow   (Green + Red)
		{RED_OFF,BLUE_OFF,GREEN_OFF}	// Black
};

void LED_PISR (void)
{
//  						Puerto 		, 	pin 			Valor
/*		GPIO_Write(LEDRGB_GPIO_PORT(red)	,	LEDRGB_PIN(red),  Led_States[count].red);
		GPIO_Write(LEDRGB_GPIO_PORT(green), 	LEDRGB_PIN(green),Led_States[count].green );
		GPIO_Write(LEDRGB_GPIO_PORT(blue), 	LEDRGB_PIN(blue), Led_States[count].blue );
*/
}






void LED_Init (void)
{
    PCRstr UserPCR;
    PORT_Init();
    GPIO_Init();
    SysTick_Init();
    registerCallback(&LED_PISR, 1);

	// Call using bit fields

    /******* RED LED PIN ********/

	UserPCR.PCR=false;			// Default All false, Set only those needed

	UserPCR.FIELD.DSE=true;
	UserPCR.FIELD.MUX=PORT_mGPIO;
	UserPCR.FIELD.IRQC=PORT_eDisabled;

	PORT_Configure2 (LEDRGB_PORT(red),LEDRGB_PIN(red),UserPCR);

	/******* GREEN LED PIN ********/
	UserPCR.PCR=false;			// Default All false, Set only those needed

	UserPCR.FIELD.DSE=true;
	UserPCR.FIELD.MUX=PORT_mGPIO;
	UserPCR.FIELD.IRQC=PORT_eDisabled;

	PORT_Configure2 (LEDRGB_PORT(green),LEDRGB_PIN(green),UserPCR);

	/******** BLUE LED PIN ***********/
	UserPCR.PCR=false;			// Default All false, Set only those needed

	UserPCR.FIELD.DSE=true;
	UserPCR.FIELD.MUX=PORT_mGPIO;
	UserPCR.FIELD.IRQC=PORT_eDisabled;

	PORT_Configure2 (LEDRGB_PORT(blue),LEDRGB_PIN(blue),UserPCR);





//  						Puerto 		, 	      pin 			 direccion

	GPIO_SetDirection(LEDRGB_GPIO_PORT(red)	 ,LEDRGB_PIN(red)	, GPIO__OUT);
	GPIO_SetDirection(LEDRGB_GPIO_PORT(green),LEDRGB_PIN(green)	, GPIO__OUT);
	GPIO_SetDirection(LEDRGB_GPIO_PORT(blue) ,LEDRGB_PIN(blue)	, GPIO__OUT);

	GPIO_Write(LEDRGB_GPIO_PORT(red)	,	LEDRGB_PIN(red),  RED_OFF);
	GPIO_Write(LEDRGB_GPIO_PORT(green), 	LEDRGB_PIN(green),GREEN_OFF);
	GPIO_Write(LEDRGB_GPIO_PORT(blue), 	LEDRGB_PIN(blue), BLUE_OFF);



}




void set_led (unsigned int led_id,bool ledstate){
    GPIOData_t data_aux;
    if(ledstate==0){
        //LED_OFF
        data_aux=GPIO__LO;//write 0
    }
    else{
        //LED_ON
        data_aux=GPIO__HI;//write 1
    }
    switch (led_id){
        case RED_LED:{
            GPIO_Write(LEDRGB_GPIO_PORT(red),LEDRGB_PIN(red),data_aux);
        }break;
        case GREEN_LED:{
            GPIO_Write(LEDRGB_GPIO_PORT(green),LEDRGB_PIN(green),data_aux);
        }break;
        case BLUE_LED:{
            GPIO_Write(LEDRGB_GPIO_PORT(blue),LEDRGB_PIN(blue),data_aux);
        }break;
    }
    return;
}

 bool get_led (unsigned int led_id){
    GPIOData_t GPIOData_aux=0;
    switch (led_id){
        case RED_LED:{
            GPIOData_aux=GPIO_Read(LEDRGB_GPIO_PORT(red),LEDRGB_PIN(red));
        }break;
        case GREEN_LED:{
            GPIOData_aux=GPIO_Read(LEDRGB_GPIO_PORT(green),LEDRGB_PIN(green));
        }break;
        case BLUE_LED:{
            GPIOData_aux=GPIO_Read(LEDRGB_GPIO_PORT(blue),LEDRGB_PIN(blue));
        }break;
        default:{
            ///ERROR CASE!
        }
    }
    if(GPIOData_aux!=0){
        return true;
    }
    else{
        return false;
    }
}
    void toggle_led (unsigned int led_id){
        set_led (led_id,!get_led (led_id));
    }








