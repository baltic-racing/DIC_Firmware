/*
 * candata.c
 *
 * Created: 23.12.2022 22:47:21
 *  Author: Ole Hannemann
 */ 



#include "candata.h"
#include "canlib.h"

uint8_t mob_databytes[4][8];

// CAN MOB from SWC.
// data layout:
// 7: Switch 4
// 6: Switch 3
// 5: Switch 2
// 4: Switch 1
// 3: Push Button right
// 2: Push Button left
// 1: Rotary Encoder right
// 0: Rotary Encoder left
struct CAN_MOB swc_mob;


// CAN MOB from Fusebox.
// data layout:
// 7: -
// 6: -
// 5: -
// 4: ADC1
// 3: ADC0
// 2: SCI_read_out
// 1: fuses msb
// 0: fuses lsb
struct CAN_MOB fusebox_mob;


struct CAN_MOB logger1_mob;


struct CAN_MOB logger2_mob;


void init_mobs(){
	swc_mob.mob_id = SWC_MOB_ID;
	swc_mob.mob_idmask = 0xff;
	swc_mob.mob_number = 0;
	
	fusebox_mob.mob_id = FUSEBOX_MOB_ID;
	fusebox_mob.mob_idmask = 0xff;
	fusebox_mob.mob_number = 0;
	
	logger1_mob.mob_id = DATALOGGER_1_MOB_ID;
	logger1_mob.mob_idmask = 0xff;
	logger1_mob.mob_number = 0;
	
	logger2_mob.mob_id = DATALOGGER_1_MOB_ID;
	logger2_mob.mob_idmask = 0xff;
	logger2_mob.mob_number = 0;
}

void can_receive(){
	can_rx(&logger1_mob, mob_databytes[LOGGER1_DATA]);
	can_rx(&logger2_mob, mob_databytes[LOGGER2_DATA]);
	can_rx(&fusebox_mob, mob_databytes[FUSEBOX_DATA]);
	can_rx(&swc_mob, mob_databytes[SWC_DATA]);
}

uint8_t get_dsp_mode(){
	return mob_databytes[SWC_DATA][DSP_MODE_BYTE];
}

uint8_t* get_mob_data(uint8_t mob){
	return mob_databytes[mob];
}


