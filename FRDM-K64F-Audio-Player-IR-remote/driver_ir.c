#include "driver_ir.h"
#include <stdbool.h> //typedef enum { false, true } bool;
/*
* THE CODE IMPLEMENT RC-5 PHILIPS PROTOCOL (MANCHESTER CODED)
* THIS PROTOCOL IMPLEMENT BIG-ENDIAN 14 SYMBOLS DATA STREAM (14 BITS)
* THERE ARE TWO SYMBOLS REPRESENTING EACH STATE:
*       '0' IS REPRESENTED BY THE VALUES 10
*       '1' IS REPRESENTED BY THE VALUES 01
*
* ONE SYMBOL IS TRANSMITED IN ONE CYCLE WHERE A 0 VALUE AND A 1 VALUE ARE SEND.
* BOTH VALUES HAVE THE SAME TIME GAP (889us)
* [CYCLE TIME= 0-TIME + 1-TIME = 2 x 889us = 1778us]
*
* THE 1 VALUE IS GENERATED WITH A SEQUENCE OF 32 PULSES BURST
* AT A FRECUENCY OF 36KHZ WITH A DUTY OF 25-33%
*
* THIS PROTOCOL HAS A START SEQUENCE TO SIMPLIFY DECODING:
* THERE ARE TWO STARTING '1' SYMBOLS (01 01) AND A TOGGLE BIT '0' (10)
* [SO THAT THE STARTING SEQUENCE WILL BE 01 01 10]
*
* AFTER THIS 3 SYMBOLS, IT FOLLOWS 5 SYMBOLS WITH THE DEVICE ADDRESS, AND A 6 SYMBOLS COMMAND
*
* THE START SEQUENCE IS QUITE USEFUL, BECAUSE IT ALLOWS TO IMPLEMENT A
* ALTERNATING EDGE INTERRUPTION AND EASILY DETECT THE VALUES WITH A
* THRESHOLD COMPARISON [(889us + 1178us) / 2 = 3/2 * 889us = 1333.5us].
* '1' '1' '0' '0' = 01 01 10 10
* '1' '1' '0' '1' = 01 01 10 01
* ITS IMPORTANT TO SEE THAT IF TWO SAME SYMBOLS ARE SEND:'0''0'(1010) OR '1''1' (0101)
* YOU WILL HAVE A TRANSITION/INTERRUPT EVERY 889us.
* BUT IF YOU SEND AN ALTERNATING SEQUENCE: '0''1' (1001) OR '1''0' (0110)
* YOU WILL HAVE A TRANSITION OF 1778us
* AND YOU WILL START WITH A RISING EDGE '1' SO DECODING IS QUITE SIMPLE
*
* TO EXPLAIN IT EASILY WILL FOLLOW THIS EXAMPLE:
* (A WILL REPRESENT THE 889us TIME GAP and B WILL REPRESENT THE 1778us)
*             START      ADDRESS           COMMAND
* SYMBOLS  '1''1''0' '1''1''1''0''0' '0''1''0''0''0''1'
* VALUES   01 01 10  01 01 01 10 10   10 01 10 10 10 01
* CHANGES  01 11 01  01 11 11 01 11   11 01 01 11 11 01 (1 ITS WHERE EDGE WILL CHANGE)
* TIME     >B AA  B   B AA AA  B AA   AA  B  B AA AA  B (A IS THE MINIMUM TIME SEPARATION OF TWO EDGE CHANGES, AND B IS THE MAXIMUM TIME SEPARATION)
* TH COMP   1  0  1   1  0  0  1  0    0  1  1  0  0  1 (1 IF TIME > THRESHOLD COMPARISON)
* RESULT    1  1  0   1  1  1  0  0    0  1  0  0  0  1 (IT CHANGES FROM 0, EVERY TIME THAT THERE IS A TH COMP)
*
* YOU NEED TO SAVE EACH VALUE AFTER TWO 'A''A' OR 'B'
* OTHER THING YOU NEED TO TAKE INTO CONSIDERATION IS THAT THERE COULD BE A GLITCH.
* A GLITCH WILL HAPEN IF TIME > B (MIN GLITCH TIME = A + B) [BUT NOT AT FIRST SYMBOL]
* GLITCH THRESHOLD = (B + (A + B)) / 2 = 5/2 * A = 5/2 * 889us = 2222.5us
* ITS RECOMENDABLE THAT IF TIME > GLITCH TH, IT IS CONSIDER THE STARTING '1'
*/

// WE USE STATIC CONST INSTEAD OF DEFINES, BECAUSE IT IS TYPESAFE AND WILL ONLY AFFECT THIS SCOPE
// TRASMISSION TIMES

// CLOCK FRECUENCY IS 50MHz
// PERIOD 1/50MHz
// TIME VALUE WITH PRESCALLER = needed_time / (PRESCALLER x PERIOD) = FRECUENCY x needed_time / PRESCALLER


static const uint16_t maximum_time_separation = 2.5 * 889 * 50  / infrared_prescaller; //GLITCH THRESHOLD (2.5 x 889us) => 2.5 * 889us * 50MHz / prescaller
static const uint16_t threshold_separation = 1.5 * 889 * 50  / infrared_prescaller; //THRESHOLD SEPARATION (1.5 x 889 uS) => 1.5 * 889us * 50MHz / prescaller

// SYMBOLS
static const uint8_t ir_total_symbols_transmitted = 14;
static const uint8_t ir_start_symbols = 3;

// RECEIVED VALUES & STATUS
static bool received_data_waiting_to_be_processed = false;
static uint16_t received_value = 0;

// SETUP
static bool setup_finished = false;


// LOCAL HEADERS
static void save_data(uint16_t transmission_data);
static inline uint16_t get_time_separation();
static inline uint16_t read_time();


bool infrared_is_data_ready(){
  return received_data_waiting_to_be_processed;
}

uint16_t infrared_read_data(){
  return received_value;
}

void infrared_interrupt(){
  static uint16_t time_separation;
  // SECURITY CHECK
  if (!setup_finished){
    std::cout << "Infrared hasn't been properly initialized. Please check setup." << std::endl;
    return;
  }

  // SYMBOL
  static bool symbol_value = false;  //TRUE = 1, FALSE = 0
  static bool is_symbol_received = false;
  // DATA RECEIVED
  static uint16_t transmission_data = 0;
  // NUMBER OF SYMBOL RECEIVED
  static uint8_t number_of_symbols_received = 0;
  // TIME
  time_separation = get_time_separation();


  if (time_separation > maximum_time_separation){  // IS A STARTING SEQUENCE / NEED TO RESTART
    symbol_value = true;  // FIRST 1 OF THE SEQUENCE
    is_symbol_received = true;
    transmission_data = 0;
    number_of_symbols_received = 0;
  }
  else if (time_separation > threshold_separation){  // CHECK IF THERE WAS A SYMBOL CHANGE
    symbol_value = !symbol_value;  // SYMBOL CHANGE
    is_symbol_received = true;  // SEE RECEIVED CONDITION AT HEADER
  }
  else{
    is_symbol_received != true;  // SEE RECEIVED CONDITION AT HEADER, NEED TO WAIT FOR TWO CALLS
  }

  // SAVE SYMBOL IF NEEDED IN transmission_data
  if (is_symbol_received){
      transmission_data <<= 1;
      transmission_data += (int)(symbol_value);
      ++number_of_symbols_received;
  }

  // SAVE RESULT IF NEEDED (WHEN transmission_data is complete)
  if (number_of_symbols_received == ir_total_symbols_transmitted){
    save_data(transmission_data); // SAVE IT IN BUFFER
    // RESTART
    symbol_value = false;
    is_symbol_received = false;
    transmission_data = 0;
    number_of_symbols_received = 0;
  }

}


static bool save_data(uint16_t transmission_data){
  static const uint16_t value_template = (1 << (ir_total_symbols_transmitted - ir_start_symbols)) - 1 ; // REPRESENT ALL 11 VALUES OF THE transmission_data
  static uint16_t previous_transmission_data = 0;

  if (transmission_data == previous_transmission_data){ //USE THE HEADER (TOGGLE BIT) & VALUE, TO SEE IF THERE IS A NEW KEY OR IS THE SAME KEY BEING PRESSED
    return false;
  }

  received_value = value_template & transmission_data;
  previous_transmission_data = transmission_data;
  received_data_waiting_to_be_processed = true;
  return true;
}


static inline uint16_t get_time_separation(){
  static uint16_t actual_time = 0, previous_time = 0, time_separation = 0;
  // MEASSURE TIME SEPARATION
  actual_time = read_time();
  time_separation = actual_time - previous_time;
  previous_time = actual_time;
  return time_separation;
}

static inline uint16_t read_time(){
    return FTM_GetCounter(FTM3, FTM_CH_5);
}


bool infrared_setup(){
  // PORT
  PCRstr UserPCR;
  UserPCR.PCR = false;			// Default All false, Set only those needed
  UserPCR.FIELD.DSE = true;
  UserPCR.FIELD.MUX = PORT_mAlt3;
  UserPCR.FIELD.IRQC = PORT_eDisabled;
  PORT_Configure2 (PORTC, infrared_port, UserPCR);

  // CLOCK
  FTM_SetPrescaler(FTM3, infrared_prescaller);	  /// BusClock=sysclk/2= 50MHz y presccaler = divx128
  FTM3->CNTIN = 0x0000;				  		  /// Free running
  FTM3->MOD = 0xFFFF;
  FTM_SetWorkingMode(FTM3, FTM_CH_5, FTM_mInputCapture);   // OC Function and toggle output
  FTM_SetInputCaptureEdge (FTM3, FTM_CH_5, FTM_eEither);  // both edges
  FTM_SetInterruptMode (FTM3, FTM_CH_5, true); // enable interrups
  FTM_StartClock(FTM3); //Select BusClk

  setup_finished = true;
}
