/*
 * candata.c
 *
 * Created: 23.12.2022 22:47:21
 *  Author: Ole Hannemann
 */ 



#include "candata.h"
#include "canlib.h"

uint8_t mob_databytes[5][8];
uint16_t max_voltage = 0;
uint16_t min_voltage = 0;
uint16_t max_temp = 0;
uint16_t min_temp = 0;


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

struct CAN_MOB ams2_mob;

struct CAN_MOB dic0_mob;


void init_mobs(){
	swc_mob.mob_id = SWC_MOB_ID;
	swc_mob.mob_idmask = 0xffff;
	swc_mob.mob_number = 0;
	
	fusebox_mob.mob_id = FUSEBOX_MOB_ID;
	fusebox_mob.mob_idmask = 0xffff;
	fusebox_mob.mob_number = 1;
	
	logger1_mob.mob_id = DATALOGGER_1_MOB_ID;
	logger1_mob.mob_idmask = 0xffff;
	logger1_mob.mob_number = 2;
	
	logger2_mob.mob_id = DATALOGGER_1_MOB_ID;
	logger2_mob.mob_idmask = 0xffff;
	logger2_mob.mob_number = 3;
	
	ams2_mob.mob_id = AMS_2_MOB_ID;
	ams2_mob.mob_idmask = 0xffff;
	ams2_mob.mob_number = 4;
	
	dic0_mob.mob_id = DIC_0_MOB_ID;
	dic0_mob.mob_idmask = 0xffff;
	dic0_mob.mob_number = 5;
	
	
}

void can_receive(){
	can_rx(&logger1_mob, mob_databytes[LOGGER1_DATA]);
	can_rx(&logger2_mob, mob_databytes[LOGGER2_DATA]);
	can_rx(&fusebox_mob, mob_databytes[FUSEBOX_DATA]);
	can_rx(&swc_mob, mob_databytes[SWC_DATA]);
	can_rx(&ams2_mob, mob_databytes[AMS2_DATA]);
}

void can_transmit(){
	can_tx(&dic0_mob, mob_databytes[DIC0_DATA]);
}

uint8_t get_dsp_mode(){
	return mob_databytes[SWC_DATA][DSP_MODE_BYTE];
}

uint8_t* get_mob_data(uint8_t mob){
	return mob_databytes[mob];
}

void can_put_data(){
	min_voltage = mob_databytes[AMS2_DATA][0] | (mob_databytes[AMS2_DATA][1] << 8);
	max_voltage = mob_databytes[AMS2_DATA][2] | (mob_databytes[AMS2_DATA][3] << 8);
	min_temp = mob_databytes[AMS2_DATA][4] | (mob_databytes[AMS2_DATA][5] << 8);
	max_temp = mob_databytes[AMS2_DATA][6] | (mob_databytes[AMS2_DATA][7] << 8);
	
}


