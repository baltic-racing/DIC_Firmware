/*
 * candata.h
 *
 * Created: 23.12.2022 22:46:01
 *  Author: Ole Hannemann
 */ 


#ifndef CANDATA_H_
#define CANDATA_H_
#include "canlib.h"

#define SWC_MOB_ID (uint16_t) 0x750
#define FUSEBOX_MOB_ID (uint16_t) 0x600
#define DATALOGGER_1_MOB_ID (uint16_t) 0x796
#define DATALOGGER_2_MOB_ID (uint16_t) 0x797
#define AMS_2_MOB_ID (uint16_t) 0x202
#define DIC_0_MOB_ID (uint16_t) 0x500

#define DSP_MODE_BYTE 0

#define SWC_DATA 0
#define FUSEBOX_DATA 1
#define LOGGER1_DATA 2
#define LOGGER2_DATA 3
#define AMS2_DATA 4
#define DIC0_DATA 5

void init_mobs();

void can_receive();

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