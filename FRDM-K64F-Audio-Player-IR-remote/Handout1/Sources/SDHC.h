/*
 * SDHC.h
 *
 *  Created on: 9/7/2018
 *      Author: Tomas & Dexter
 */
#ifndef SDHC_H_
#define SDHC_H_

#include "Template/LDM.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define SDHC_FIFO_BUFFER_SIZE               16
#define SDHC_BLOCK_SIZE                     512
#define SDHC_INITIALIZATION_MAX_CNT 100000

//define possible commands skipping those reserved
typedef enum CMD_INDEX {GO_IDLE_STATE,          SEND_OP_COND,          ALL_SEND_CID,          SET,            SET_DSR,           IO_SEND_OP_COND,
						SWITCH_FUNC, 		    SELECT_CARD, 		   SEND_EXT_CSD, 		  SEND_CSD,       SEND_CID,          READ_DAT_UNTIL_STOP,
						STOP_TRANSMISSION,      SEND_STATUS,           GO_INACTIVE_STATE=15,  SET_BLOCKLEN,   READ_SINGLE_BLOCK, READ_MULTIPLE_BLOCK,
						WRITE_DAT_UNTIL_STOP=20,WRITE_BLOCK=24,        WRITE_MULTIPLE_BLOCK,  PROGRAM_CID,    PROGRAM_CSD,       SET_WRITE_PROT,
						CLR_WRITE_PROT,         SEND_WRITE_PROT,       TAG_SECTOR_START=32,   TAG_SECTOR_END, UNTAG_SECTOR,      TAG_ERASE_GROUP_START,
						TAG_ERASE_GROUP_END,    UNTAG_ERASE_GROUP,     ERASE, FAST_IO,        GO_IRQ_STATE,   LOCK_UNLOCK=42,    IO_RW_DIRECT=52,
						IO_RW_EXTENDED,         APP_CMD=55,            GEN_CMD,               RW_MULTIPLE_REGISTER=60,           RW_MULTIPLE_BLOCK,

	//special commands
						SET_BUS_WIDTH=0x40+6, SD_STATUS=0x40+13, SEND_NUM_WR_SECTORS=0x40+22, SET_WR_BLK_ERASE_COUNT, SD_APP_OP_COND=0x40+41, SET_CLR_CARD_DETECT,SEND_SCR=0x40+51
						} CMD_INDEX;

typedef enum RSPS_TYPE {NO_RESPONSE, RESPONSE_136, RESPONSE_48, RESPONSE_48_CHK_BUSY} RSPS_TYPE;
typedef enum RSPS{RESPONSE_OK,NO_RSPS,CARD_REMOVAL,INDEX_ERROR,END_BIT_ERROR,CRC_ERROR,GENERAL_ERROR} RSPS;


//struct with the transfer type register (SDHC_XFERTYP) fields
typedef struct {

	//these vary according to the command index
	CMD_INDEX CMDINX;	//index, number of command, 6 bits
	uint8_t CMDTYP;		//only 4 bits (lower nibble) needed
	bool DPSEL;			//This bit is set to 1 to indicate that data is present and shall be transferred using the DAT line.
	bool CICEN;			//Command Index Check Enable: If this bit is set to 1, the SDHC will check the index field in the response to see if it has the same value as the command index
	bool CCCEN;			//Command CRC Check Enable: If this bit is set to 1, the SDHC shall check the CRC field in the response.
	uint8_t RSPTYP;		//only 4 bits (upper nibble) needed: 0 no response, 1 response length 136, 2 response length 48, 3 response length 48, check busy after response
	bool DTDSEL;		//Defines the direction of DAT line data transfers. The bit is set to 1 by the host driver to transfer data from
						//the SD card to the SDHC

	bool DMAEN;			//if internal DMA is used
	bool MSBSEL;		//for multiblock transfer
	bool BCEN;			//for finite block transfer
	bool AC12EN;		//to do an automatic command 12 (stop transmission) when the last block has completed

}XFERTYP_Field;


void SDHC_Field_Clear(XFERTYP_Field* Field);

void SDHC_Init(void);
bool SDHC_detectsdcard(void);
bool SDHC_detectsdcardinterrupt(void);
bool SDHC_isCardInserted(void);
bool SDHC_isReady(void);
RSPS SDHC_send_command(uint32_t cmd_arg, XFERTYP_Field field);
void SDHC_ConfigurePins(bool enable);


bool SDHC_dma_update();
unsigned char* SDHC_get_sector_data(uint32_t sector);

#endif /* SDHC_H_ */
