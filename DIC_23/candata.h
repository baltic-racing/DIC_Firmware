/*
 * candata.h
 *
 * Created: 23.12.2022 22:46:01
 *  Author: Ole Hannemann
 */ 


#ifndef CANDATA_H_
#define CANDATA_H_
#include "canlib.h"

	
void init_can_mobs();
void can_receive();
uint8_t get_dsp_mode();


#endif /* CANDATA_H_ */