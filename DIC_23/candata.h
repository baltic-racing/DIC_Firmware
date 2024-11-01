/*
 * candata.h
 *
 * Created: 23.12.2022 22:46:01
 *  Author: Ole Hannemann
 */ 

#ifndef CANDATA_H_
#define CANDATA_H_
#include "canlib.h"
#include "portextender.h"

#define AMS_0_MOB_ID (uint16_t) 0x200
#define AMS_1_MOB_ID (uint16_t) 0x201

#define SHR_MOB_ID (uint16_t) 0x400
#define SHL_MOB_ID (uint16_t) 0x490 //0x410 eigentliche ID
#define SHB_0_MOB_ID (uint16_t) 0x420
#define SHB_1_MOB_ID (uint16_t) 0x421


#define DIC_MOB_ID (uint16_t) 0x500

#define FUSEBOX_MOB_ID (uint16_t) 0x600

#define SWC_MOB_ID (uint16_t) 0x750

#define LOGGER_0_MOB_ID (uint16_t) 0x796
#define LOGGER_1_MOB_ID (uint16_t) 0x797
#define LOGGER_2_MOB_ID (uint16_t) 0x798
#define LOGGER_3_MOB_ID (uint16_t) 0x799


#define INV0_0_MOB_ID (uint16_t) 0x00
#define INV0_1_MOB_ID (uint16_t) 0x45A

#define INV1_0_MOB_ID (uint16_t) 0x00
#define INV1_1_MOB_ID (uint16_t) 0x44A


#define DSP_MODE_BYTE 0

#define AMS0_DATA 0
#define AMS1_DATA 1

#define SHR_DATA 2
#define SHL_DATA 3
#define SHB_DATA_0 4

#define DIC_DATA 5

#define FUSEBOX_DATA 6

#define SWC_DATA 7

#define INV00_DATA 8
#define INV01_DATA 9

#define INV10_DATA 10
#define INV11_DATA 11

#define SHB_DATA_1 12


void init_mobs();

void can_receive();
void can_transmit();

void can_put_data();

uint8_t get_dsp_mode();

//this function will return all databytes from a mob
uint8_t* get_mob_data(uint8_t mob);
//usage example:
//
// printf("%d", get_mob_data(SWC_DATA)[1]);
//
// this would print the value of the right rotary encoder.
//
//BEWARE: this function returns a pointer and you must know the array size
//this should be ok though since all our mobs are 8 byte long
//
//if you want to be funny you could create defines for all datapoints
// eg.:
// #define RIGHT_ENCODER get_mob_data(SWS_DATA)[1]
// would let you get the right encoder value directly
// you could then print that value with something like
// printf("%d", RIGHT_ENCODER);


#endif /* CANDATA_H_ */