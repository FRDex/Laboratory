/*
 * SDHC.c
 *
 *  Created on: 9/7/2018
 *      Author: tomh6
 */
#include "SDHC.h"
#include "PORT.h"
#include "GPIO.h"
#include "DAC.h"
//#include "PIN.h"


#define PIN_SDHC_D1 	0   //PTE0
#define PIN_SDHC_D0 	1   //PTE1
#define PIN_SDHC_DCLK 	2   //PTE2
#define PIN_SDHC_CMD 	3   //PTE3
#define PIN_SDHC_D3 	4   //PTE4
#define PIN_SDHC_D2 	5   //PTE5
#define PIN_SD_DETECT   6   //PTE6


#define PORT_SDHC PORTE
#define GPIO_SDHC PTE

unsigned long address; //address of the SD card

static bool CardInserted=false;
static bool SDHC_Ready=false;
static RSPS readRes = 0;

void SDHC_Init(void){

	//INICIALIZACION
	SIM_SOPT2 |= SIM_SOPT2_SDHCSRC(0);	//just in case the clock source is wrong
	SIM_SCGC3 |= SIM_SCGC3_SDHC_MASK;	//clock gating for SDHC
    SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;	//PORTE clock gating

    SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;

    MPU->CESR &= ~MPU_CESR_VLD_MASK;	//MPU is disabled. All accesses from all bus masters are allowed

    SDHC_ConfigurePins(false);

    SDHC->SYSCTL = SDHC_SYSCTL_RSTA_MASK | SDHC_SYSCTL_SDCLKFS(0x80);	//reset system
    while((SDHC->SYSCTL)&SDHC_SYSCTL_RSTA_MASK){}						//wait until reset is complete

	SDHC->PROCTL |= SDHC_PROCTL_D3CD_MASK;
	SDHC->VENDOR = 0;
    SDHC->WML = SDHC_WML_RDWML(SDHC_FIFO_BUFFER_SIZE) | SDHC_WML_WRWML(SDHC_FIFO_BUFFER_SIZE);
  //  SDHC->WML = SDHC_WML_RDWML(SDHC_FIFO_BUFFER_SIZE);		//for reading, the watermark level is 16 words
    SDHC->BLKATTR = SDHC_BLKATTR_BLKCNT(1) | SDHC_BLKATTR_BLKSIZE(SDHC_BLOCK_SIZE);	//the size of reading blocks is 512 (a section in the SD card)

    //SET BAUD RATE
    //SDHC_SetBaudrate(SDHC_400KHZ);


    SDHC_ConfigurePins(true);

    SDHC->IRQSTAT = 0xFFFF;		//clear all stat of IRQSTAT
	//SDHC->IRQSTATEN |= SDHC_IRQSTATEN_CINTSEN_MASK;		//Card interrupt status enable
	//SDHC->IRQSTATEN |= SDHC_IRQSTATEN_CINSEN_MASK;		//Card insert status enable
	SDHC->IRQSTATEN |= SDHC_IRQSTATEN_DMAESEN_MASK | SDHC_IRQSTATEN_AC12ESEN_MASK | SDHC_IRQSTATEN_DEBESEN_MASK | 	//all other interrupts
					   SDHC_IRQSTATEN_DCESEN_MASK  | SDHC_IRQSTATEN_DTOESEN_MASK  | SDHC_IRQSTATEN_CIESEN_MASK  |
					   SDHC_IRQSTATEN_CEBESEN_MASK | SDHC_IRQSTATEN_CCESEN_MASK   | SDHC_IRQSTATEN_CTOESEN_MASK |
					   SDHC_IRQSTATEN_BRRSEN_MASK  | SDHC_IRQSTATEN_BWRSEN_MASK   | SDHC_IRQSTATEN_DINTSEN_MASK |
					   SDHC_IRQSTATEN_CRMSEN_MASK  | SDHC_IRQSTATEN_TCSEN_MASK    | SDHC_IRQSTATEN_CCSEN_MASK;

	//SDHC->IRQSIGEN |= SDHC_IRQSIGEN_CINTIEN_MASK;
	//SDHC->IRQSIGEN |= SDHC_IRQSIGEN_CINSIEN_MASK;

    SDHC->SYSCTL |= SDHC_SYSCTL_INITA_MASK;			//init the clocks
    while (SDHC->SYSCTL & SDHC_SYSCTL_INITA_MASK)
        { };

    SDHC->IRQSTAT |= SDHC_IRQSTAT_CRM_MASK;

	//NVIC_EnableIRQ(SDHC_IRQn);


	//RESET
	XFERTYP_Field Conf = {0};
	Conf.CMDINX=GO_IDLE_STATE;
	Conf.RSPTYP=NO_RESPONSE;
	readRes = SDHC_send_command(0, Conf);

	//SET INTERFACE
	Conf.CMDINX=SEND_EXT_CSD;
	Conf.CCCEN=1;
	Conf.CICEN=1;
	Conf.RSPTYP=RESPONSE_48;
	readRes = SDHC_send_command(0x000001AA, Conf);

	//VOLTAGE VALIDATION
	Conf.CMDINX=APP_CMD;
	readRes = SDHC_send_command(0, Conf);
	Conf.CMDINX=SD_APP_OP_COND;
	Conf.CCCEN=0;
	Conf.CICEN=0;
	readRes = SDHC_send_command(0, Conf);

    if (SDHC->CMDRSP[0] & 0x300000)
    {
      unsigned long condition = 0x00300000;
      int i = 0;
      while ((0 == (SDHC->CMDRSP[0] & 0x80000000)) && (i < SDHC_INITIALIZATION_MAX_CNT)){
    	  	  Conf.CMDINX=APP_CMD;
			  Conf.CCCEN=1;
			  Conf.CICEN=1;
			  SDHC_send_command(0, Conf);
			  Conf.CMDINX=SD_APP_OP_COND;
			  Conf.CCCEN=0;
			  Conf.CICEN=0;
			  readRes = SDHC_send_command(condition, Conf);
			  i++;
      }
      int o= i;
    }

	//IDENTIFY
	Conf.CMDINX=ALL_SEND_CID;
	Conf.CCCEN=1;
	Conf.RSPTYP=RESPONSE_136;
	readRes = SDHC_send_command(0, Conf);

	//CARD ADDRESS
	Conf.CMDINX=SET;
	Conf.CICEN=1;
	Conf.RSPTYP=RESPONSE_48;
	readRes = SDHC_send_command(0, Conf);

    address = SDHC_CMDRSP0 & 0xFFFF0000;

	//SELECT CARD
	Conf.CMDINX=SELECT_CARD;
	Conf.RSPTYP=RESPONSE_48_CHK_BUSY;
	readRes = SDHC_send_command(address, Conf);

	// Set Block Size to 512
	Conf.CMDINX=SET_BLOCKLEN;
	Conf.RSPTYP=RESPONSE_48;
	readRes = SDHC_send_command(SDHC_BLOCK_SIZE, Conf);

	// Set Bus Width of data to 4 bit
	Conf.CMDINX=APP_CMD;
	readRes = SDHC_send_command(address, Conf);
	Conf.CMDINX=SWITCH_FUNC;
	readRes = SDHC_send_command(2, Conf);

	// Set Data bus width also in SDHC controller
	SDHC->PROCTL &= (~ SDHC_PROCTL_DTW_MASK);
	SDHC->PROCTL |= SDHC_PROCTL_DTW(1);

	SDHC_ConfigurePins(false);

	//set baudrate 25MHz
	//SDHC_SetBaudrate(SDHC_25MHZ);

	SDHC_ConfigurePins(true);

	SDHC_Ready=true;


}
static bool doonce=0;
void SDHC_ConfigurePins(bool enable){
	PCRstr UserPCR;		//to configure pins
	UserPCR.PCR=false;		//configure everything false then select corresponding flags

	if(enable==true){
		UserPCR.FIELD.DSE=true;	//drive strength enagle
		UserPCR.FIELD.PE=true;	//pull enable
		UserPCR.FIELD.PS=true;	//pull select
		UserPCR.FIELD.MUX=PORT_mAlt4;		// SDHC
		UserPCR.FIELD.IRQC=PORT_eDisabled;  // No Irqs from port
	}

	PORT_Configure2(PORT_SDHC,PIN_SDHC_D0,UserPCR); //Setup SDHC pins
	PORT_Configure2(PORT_SDHC,PIN_SDHC_D1,UserPCR);
	PORT_Configure2(PORT_SDHC,PIN_SDHC_D2,UserPCR);
	PORT_Configure2(PORT_SDHC,PIN_SDHC_D3,UserPCR);
	PORT_Configure2(PORT_SDHC,PIN_SDHC_DCLK,UserPCR);
	PORT_Configure2(PORT_SDHC,PIN_SDHC_CMD,UserPCR);
	if (!doonce){
		UserPCR.PCR = false;
		UserPCR.FIELD.PE = true;
		UserPCR.FIELD.MUX = PORT_mGPIO;
		UserPCR.FIELD.IRQC=PORT_eDisabled;
		PORT_Configure2(PORT_SDHC, PIN_SD_DETECT, UserPCR);
		GPIO_SetDirection(GPIO_SDHC, PIN_SD_DETECT, GPIO__IN);
		doonce=1;
	}

//	{ PORTE,  PTE,   6, PIN_IN_PULLDOWN,   GPIO__IN,  GPIO__LO, GPIO__HI } // PIN_SD_DETECT
	//    port,  gpio, PIN,   portConfig,       dir,     onLevel,  initLevel

}

bool detectsdcard(void){
//	if(Pin_Read(PIN_SD_DETECT)==true){	//detect pin is normally high
	GPIOData_t pinVal = GPIO_Read(GPIO_SDHC, PIN_SD_DETECT);
 	if(!pinVal){		// mirar comparacion en PIN.c
		CardInserted=false;
		return false;
	}else{
		CardInserted=true;
		return true;
	}
}

bool detectsdcardinterrupt(void){	//detect the sd card by an IRQ generated by K64F

	if(((SDHC->IRQSTAT)&SDHC_IRQSTAT_CINS_MASK)==0)	//detect pin is normally high
		return false;
	else return true;
}

RSPS SDHC_send_command(uint32_t cmd_arg, XFERTYP_Field field)
{

    while ((SDHC->PRSSTAT & SDHC_PRSSTAT_CIHB_MASK) || (SDHC->PRSSTAT & SDHC_PRSSTAT_CDIHB_MASK))	//wait until DAT and CMD line are empty
    { };

	//put the command argument
	SDHC->CMDARG = cmd_arg;

	//set all the fields or XFERTYP
	SDHC->XFERTYP = SDHC_XFERTYP_CMDINX(field.CMDINX) | SDHC_XFERTYP_CMDTYP(field.CMDTYP) | SDHC_XFERTYP_DPSEL(field.DPSEL) |
					SDHC_XFERTYP_CICEN(field.CICEN)   | SDHC_XFERTYP_CCCEN(field.CCCEN)   |
					SDHC_XFERTYP_RSPTYP(field.RSPTYP) | SDHC_XFERTYP_DTDSEL(field.DTDSEL) |
					SDHC_XFERTYP_DMAEN(field.DMAEN)   | SDHC_XFERTYP_MSBSEL(field.MSBSEL) |
					SDHC_XFERTYP_BCEN(field.BCEN)     | SDHC_XFERTYP_AC12EN(field.AC12EN);

	unsigned long DATA_INTERRUPT_MASK = SDHC_IRQSTAT_CIE_MASK | SDHC_IRQSTAT_CEBE_MASK | SDHC_IRQSTAT_CCE_MASK | SDHC_IRQSTAT_CC_MASK;
	unsigned long result=0;

	while(result==0){
		result = (SDHC->IRQSTAT) & DATA_INTERRUPT_MASK;
	}

	//first check there are no errors
	if(result != SDHC_IRQSTAT_CC_MASK){
        SDHC_IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CIE_MASK | SDHC_IRQSTAT_CEBE_MASK |
        				SDHC_IRQSTAT_CCE_MASK  | SDHC_IRQSTAT_CC_MASK;			//clear all these bits that generated the interrupt

        switch (result & DATA_INTERRUPT_MASK){
        	case SDHC_IRQSTAT_CIE_MASK:
			case SDHC_IRQSTAT_CIE_MASK | SDHC_IRQSTAT_CC_MASK:
				return INDEX_ERROR;
			break;

			case SDHC_IRQSTAT_CEBE_MASK:
			case SDHC_IRQSTAT_CEBE_MASK | SDHC_IRQSTAT_CC_MASK:
				return END_BIT_ERROR;
			break;

			case SDHC_IRQSTAT_CCE_MASK:
			case SDHC_IRQSTAT_CCE_MASK| SDHC_IRQSTAT_CC_MASK:
				return CRC_ERROR;
			break;

			default:
				return GENERAL_ERROR;
			break;
		}
	}

	//then check the card was not removed
    if (SDHC->IRQSTAT & SDHC_IRQSTAT_CRM_MASK)
    {
        SDHC->IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CRM_MASK | SDHC_IRQSTAT_CC_MASK;	//clear all those bits
        return CARD_REMOVAL;
    }

    //then check if there is not a timeout general interrupt
    if (SDHC->IRQSTAT & SDHC_IRQSTAT_CTOE_MASK)
    {
    	SDHC->IRQSTAT |= SDHC_IRQSTAT_CTOE_MASK | SDHC_IRQSTAT_CC_MASK;		//clear all those bits
        return NO_RSPS;
    }

    //great! if we're here, it means everything is good and I got a response from the card!
    SDHC->IRQSTAT |= SDHC_IRQSTAT_CC_MASK;	//write 1 to clear CC bit

    return RESPONSE_OK;


}


void SDHC_sector_read(unsigned char* dataaddress, unsigned long sector){
    sector *= 512;
    unsigned long* pData = (unsigned long*) dataaddress;

	XFERTYP_Field MyConfig = {0};
	MyConfig.CMDINX=READ_SINGLE_BLOCK;
	MyConfig.RSPTYP=RESPONSE_48;
	MyConfig.CICEN=1;
	MyConfig.CCCEN=1;
	MyConfig.DPSEL=1;
	MyConfig.DTDSEL=1;
	MyConfig.DMAEN=1;


	SDHC->IRQSTAT = 0xffff;	//clear all IRQ
	while(SDHC->PRSSTAT & SDHC_PRSSTAT_DLA_MASK) {};	//wait until data line is active
	  SDHC->IRQSTAT |= SDHC_IRQSTAT_TC_MASK;				//enable transfer complete interrupt
	  SDHC->DSADDR  = (unsigned long) pData;

	SDHC->BLKATTR &= ~(SDHC_BLKATTR_BLKCNT_MASK);	//set block count in 1
	SDHC->BLKATTR |= SDHC_BLKATTR_BLKCNT(1);


	readRes = SDHC_send_command(sector, MyConfig);
}


__ISR__ SDHC_IRQHandler (void)
{

	//NVIC_DisableIRQ(SDHC_IRQn);
	//NVIC_EnableIRQ(SDHC_IRQn);
	//(SDHC->IRQSTAT) |= SDHC_IRQSTAT_CINS_MASK;
	SDHC->IRQSIGEN &= ~(SDHC_IRQSIGEN_CINSIEN_MASK);	//ignore afterwards card insertions
	SDHC->IRQSTATEN &= ~(SDHC_IRQSTATEN_CINSEN_MASK);	//ignore afterwards card insertions

	SDHC->IRQSIGEN &= ~(SDHC_IRQSIGEN_CINTIEN_MASK);	//Clear CINTIEN bit in IRQSTATEN (CINTSEN) and IRQSIGEN.
	SDHC->IRQSTATEN &= ~(SDHC_IRQSTATEN_CINTSEN_MASK);	//

	(SDHC->IRQSTAT) |= SDHC_IRQSTAT_CINT_MASK;			//Reset the interrupt factors in the SDIO card and write 1 to clear CINT interrupt in IRQSTAT.

	SDHC->PROCTL &= ~(SDHC_PROCTL_D3CD_MASK);			//Clear and then set D3CD bit in the PROCTL register.
	(SDHC->PROCTL) |= SDHC_PROCTL_D3CD_MASK;

	SDHC->IRQSIGEN |= SDHC_IRQSIGEN_CINTIEN_MASK;	//Clear CINTIEN bit in IRQSTATEN (CINTSEN) and IRQSIGEN.
	SDHC->IRQSTATEN |= SDHC_IRQSTATEN_CINTSEN_MASK;	//
}



void SDHC_Field_Clear(XFERTYP_Field* Field){
	Field->AC12EN=0;
	Field->BCEN=0;
	Field->CCCEN=0;
	Field->CICEN=0;
	Field->CMDINX=0;
	Field->CMDTYP=0;
	Field->DMAEN=0;
	Field->DPSEL=0;

}

bool SDHC_isCardInserted(void){
	return CardInserted;
}

bool SDHC_isReady(void){
	return SDHC_Ready;
}







