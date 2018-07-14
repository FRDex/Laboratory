/*
 * PLAYER.c
 *
 *  Created on: 12/7/2018
 *      Author: El Diablo & Dexter
 */
 
#include "PLAYER.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "IR_driver.h"
#include "SysTick.h"
#include "SDHC.h"

/*WAV file HEADER:*/
#define SECTOR_SIZE 512
#define RIFF 0              //"RIFF" chunk descriptor 4 bytes from 0 to 3. big endian.
#define F_SIZE 4            //Total file size -8 bytes. 4 bytes from 4 to 7 little endian.
#define FORMAT 8            //File format, contains the letter "WAVE" for WAV files, 4 bytes from 8 to 11. big endian.
#define FTM 12              //contains the letters "ftm ", 4 bytes from 12 to 15. big endian.
#define FORMAT_INFO_SIZE 16 //size of format info block, 4 bytes from 16 to 19. little endian.
#define F_TYPE 20           //Type of format (1 is PCM, other values indicates some form of compression), 2 byte interger.little endian.
#define NUM_CHANNELS 22     // Number of channels Mono = 1, Stereo = 2, etc. 22/23. little endian.
#define SAMPLE_RATE	24      // Sample rate 4 bytes from 24 to 27. little endian.
#define BYTE_RATE	28        // Byte rate =  Sample_rate*Num_Channels*BitsPerSample/8, 4 bytes 28 to 31. little endian.
#define BLOCK_ALING	32      // Number of bytes per sample including all channels =NUM_CHANNELS * BitsPerSample/8. 2 bytes 32/33. little endian.
#define BITS_PER_SAMPLE	34  // 8 bits = 8, 16 bits = 16, 34/35, little endian.
#define DATA 36             // Here should appear the word "DATA" big endian, if the 4 bytes are different from DATA read the next four and advance that number of bytes trying to find "DATA"
#define DATA_SIZE 40        // Size of the data we want to read. 40 to 43, in 44 the samples should start.
#define DATA_START 44
/*END HEADER directory.*/

#define FAST_FORWARD_REWIND_OFFSET 1000
#define number_of_songs  3

//	IR BUTTONS	//
typedef enum IR_BUTTONS {	PLAY,	PAUSE,	STOP,	FORWARD,	BACKWARD,	NEXT,	PREVIOUS,	VOLUME_UP,	VOLUME_DOWN	}IR_BUTT;

static uint16_t ISR_index;
static const uint32_t song_list[number_of_songs] = {839, 839, 839};

typedef struct {
  // SONG VARAIBLES
  uint8_t song_list_index;
  uint32_t sector_start;
  uint32_t sector_end;
  uint32_t playing_sector;
  uint32_t playing_sample;
  uint16_t bit_resolution; // FALSE INDICATE 8 BIT RESOLUTION
  uint16_t samples_start;
  uint32_t sampling_rate;
  // PLAYING VARIABLES
  int8_t volume_reduction; // 0 - 7
  bool playing;
} song_structure;


static song_structure current_song = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

bool player_init(void);
void play_isr(void);
//bool player_play(int32_t song_address);
static bool player_pause(void);
static bool player_stop(void);
static bool player_forward(void);
static bool player_backward(void);
static bool player_next(void);
static bool player_previous(void);
static bool player_volume_up(void);
static bool player_volume_down(void);
bool player_command(uint16_t command);
uint32_t read_handler(bool big_endian, uint16_t start, uint16_t end, unsigned char * array);
static bool increase_playing_indexes(int32_t sample_offset);

bool player_init(){
	ISR_index = registerCallback(&play_isr, 44100); //
  player_play(song_list[current_song.song_list_index]);
	return true;
}

void play_isr(){
  current_song.playing = true;
	if(infrared_is_data_ready()){	//esto se podria hacer con un ISR distinto o saltear el if algunos ciclos porque no tiene que ser tan rapido como la reproduccion de los samples.
		player_command(infrared_read_data());
	}
//	if(player_running){
		//cargar sample
		//si es la ultima del sector cargar el proximo sector
		//si termino la canci�n cargar la pr�xima
		//enviar sample actual al DAC
//	}
}


bool player_play(int32_t song_address){ //Se debe llamar cuando inicia una nueva cancion
    player_stop();

  	unsigned char song_header[SECTOR_SIZE];
    current_song.sector_start = song_address;
    SDHC_sector_read(song_header, current_song.sector_start);

    current_song.sampling_rate = read_handler(false, SAMPLE_RATE, SAMPLE_RATE + 4, song_header);
    current_song.bit_resolution = read_handler(false, BITS_PER_SAMPLE, BITS_PER_SAMPLE + 2, song_header);

    uint32_t file_size = read_handler(false, F_SIZE, F_SIZE + 4, song_header);
    current_song.sector_end = current_song.sector_start + (file_size + 8) / SECTOR_SIZE;	//last sector

    uint32_t data_string = read_handler(true, DATA, DATA + 4, song_header);

    static unsigned char data[4] = {'d', 'a', 't', 'a'};
    uint32_t data_string_value = read_handler(true, 0, 4, data);

    uint32_t data_offset = 0;
  	if(data_string != data_string_value){	//	CHECK IF THERE IS AN OFFSET
      data_offset = read_handler(false, DATA_SIZE, DATA_SIZE + 4, song_header);
      data_string = read_handler(true, DATA + data_offset, DATA + 4 + data_offset, song_header);
  	}

  	current_song.samples_start = DATA_START + data_offset + (DATA_START - DATA);
    current_song.playing_sector = current_song.sector_start;
    current_song.playing_sample = current_song.samples_start;
    setCallback_Freq(ISR_index, current_song.sampling_rate);
    current_song.playing = true;

  	return true;

}

static bool player_pause(){
	current_song.playing = false;
	return true;
}

static bool player_stop(){
	player_pause();
	current_song.playing_sector = current_song.sector_start;
	current_song.playing_sample = current_song.samples_start;
	return true;
}

static bool player_forward(){
  return increase_playing_indexes(FAST_FORWARD_REWIND_OFFSET);
}

static bool player_backward(){
  return increase_playing_indexes(-FAST_FORWARD_REWIND_OFFSET);
}

static bool player_next(){
  if((++current_song.song_list_index) >= number_of_songs){
    current_song.song_list_index = 0;
  }
  player_play(song_list[current_song.song_list_index]);
  return true;
}

static bool player_previous(){
  if((--current_song.song_list_index) < 0){
    current_song.song_list_index = number_of_songs - 1;
  }
  player_play(song_list[current_song.song_list_index]);
  return true;
}

static bool player_volume_up(){
  if((--current_song.volume_reduction) < 0){
    current_song.volume_reduction = 0;
  }
  return true;
}

static bool player_volume_down(){
  if((++current_song.volume_reduction) > current_song.bit_resolution){
    current_song.volume_reduction = current_song.bit_resolution;
  }
  return true;
}

bool player_command(uint16_t command){
  switch (command){
    case PLAY:{
      player_play(current_song.sector_start);
    }
    break;
    case PAUSE:{
      player_pause();
    }
    break;
    case STOP:{
      player_stop();
    }
    break;
    case FORWARD:{
      player_forward();
    }
    break;
    case BACKWARD:{
      player_backward();
    }
    break;
    case NEXT:{
      player_next();
    }
    break;
    case PREVIOUS:{
      player_previous();
    }
    break;
    case VOLUME_UP:{
      player_volume_up();
    }
    break;
    case VOLUME_DOWN:{
      player_volume_down();
    }
    break;
    default:{
      return false;
    }
  }
  return true;
}

uint32_t read_handler(bool big_endian, uint16_t start, uint16_t end, unsigned char * array){
  uint32_t value = 0;
  if (big_endian){
	  for (uint16_t index = start; index < end; ++index){
		  value <<= 8;
		  value += array[index];
	  }
  }
  else{
    for (uint16_t index = end - 1; index >= start; --index){
      value <<= 8;
      value += array[index];
    }
  }
  return value;
}

static bool increase_playing_indexes(int32_t sample_offset){
  int32_t actual_sample = current_song.playing_sample;
  int32_t actual_sector = current_song.playing_sector;

  actual_sample += sample_offset;

  while (actual_sample >= SECTOR_SIZE){
    actual_sample -= SECTOR_SIZE;
    ++actual_sector;
  }

  while (actual_sample < 0){
    actual_sample += SECTOR_SIZE;
    --actual_sector;
  }

  actual_sector = actual_sector < current_song.sector_start ? current_song.sector_start : actual_sector;
  actual_sector = actual_sector > current_song.sector_end ? current_song.sector_end : actual_sector;

  current_song.playing_sample = actual_sample;
  current_song.playing_sector = actual_sector;

  return true;
}
