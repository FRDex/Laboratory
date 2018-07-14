
#include "App.h"

#include "PORT.h"
#include "GPIO.h"
#include "FTM.h"
#include "IR_driver.h"
#include "SDHC.h"
#include "DAC.h"
static unsigned char buffer1[512];
static unsigned char buffer2[512];

void App_Init (void)
{
	PORT_Init();
	GPIO_Init();
	FTM_Init();
	DAC_Init();
	infrared_setup();
	SDHC_Init();
}

uint16_t values[5];
int index = 0;
void App_Run (void)
{
	unsigned long sector1= 615;
	unsigned long sector2 = 79049;
	bool newCard = isCardInserted();
	if(newCard && isSDHCReady()){
		block_read(buffer1,sector1);
		block_read(buffer2,sector2);
		index=index;
	}
	else
		detectsdcard();
/*	if((infrared_is_data_ready()) && (index<5)){
		values[index] = infrared_read_data();
		++index;
	}
	else if (index>=5){
		index=0;
	}*/

}
