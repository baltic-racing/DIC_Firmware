/*
 * candata.h
 *
 * Created: 23.12.2022 22:46:01
 *  Author: Ole Hannemann
 */ 


#ifndef CANDATA_H_
#define CANDATA_H_
#include "canlib.h"

#define AMS_0_MOB_ID (uint16_t) 0x200
#define AMS_1_MOB_ID (uint16_t) 0x201

#define SHR_MOB_ID (uint16_t) 0x400
#define SHF_MOB_ID (uint16_t) 0x401
#define SHB_MOB_ID (uint16_t) 0x420

#define DIC_MOB_ID (uint16_t) 0x500

#define FUSEBOX_MOB_ID (uint16_t) 0x600

#define SWC_MOB_ID (uint16_t) 0x750

#define vcu_0_mob_ID (uint16_t) 0x300

#define INV0_0_MOB_ID (uint16_t) 0x44A
#define INV0_1_MOB_ID (uint16_t) 0x453



#define DSP_MODE_BYTE 1
#define PBL 2
#define PBR 3
#define SWTL 4
#define SWTR 6

#define AMS0_DATA 0
#define AMS1_DATA 1

#define SHR_DATA 2
#define SHF_DATA 3
#define SHB_DATA 4

#define DIC_DATA 5

#define FUSEBOX_DATA 6

#define SWC_DATA 7

#define VCU_DATA 8

#define INV00_DATA 9
#define INV01_DATA 10



void init_mobs();

void can_receive();
void can_transmit();

void can_put_data();

uint8_t get_dsp_mode();

uint8_t get_swc_mode();

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