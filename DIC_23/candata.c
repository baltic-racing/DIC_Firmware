/*
 * candata.c
 *
 * Created: 23.12.2022 22:47:21
 *  Author: Ole Hannemann
 */ 



#include "candata.h"
#include "canlib.h"

uint8_t mob_databytes[5][8];

uint16_t ts_voltage = 0;
uint16_t ts_current = 0;
uint16_t state_of_charge = 0;
uint8_t ams_error = 0;
uint8_t imd_error = 0;
uint8_t can_ok = 0;
uint8_t precharge_active = 0;
uint8_t TS_RDY = 0;

uint16_t bms_max_voltage = 0;
uint16_t bms_min_voltage = 0;
uint16_t bms_max_temp = 0;
uint16_t bms_min_temp = 0;

uint16_t APPS = 0;

uint16_t cooling_1 = 0;
uint16_t cooling_2 = 0;
uint16_t cooling_temp = 0;
uint16_t cooling_temp_deg = 0;

uint8_t Ready_2_Drive = 0;
uint8_t TS_ON = 0;
uint8_t SDCIDIC = 0;

uint16_t battery_voltage = 0;
uint8_t SDCIFB = 0;
uint16_t fuse_readout = 0;

uint16_t gps_speed = 0;

uint16_t ERPM_0 = 0;
uint16_t ERPM_1 = 0;

uint16_t mcu_temp_0 = 0;
uint16_t motor_temp_0 = 0;
uint8_t fault_code_0 = 0;

uint16_t mcu_temp_1 = 0;
uint16_t motor_temp_1 = 0;
uint8_t fault_code_1 = 0;


// CAN MOB 0 from AMS.
// data layout:
// 7: X
// 6: AMS_ERR | IMD_ERR | CAN_OK | PRECHARGE | TS_RDY | X | X | X
// 5: State of Charge
// 4: State of Charge
// 3: TS Current
// 2: TS Current
// 1: TS Voltage
// 0: TS Voltage
struct CAN_MOB ams0_mob;

// CAN MOB 1 from AMS.
// data layout:
// 7: BMS Temperature MAX
// 6: BMS Temperature MAX
// 5: BMS Temperature MIN
// 4: BMS Temperature MIN
// 3: BMS Voltage MAX
// 2: BMS Voltage MAX
// 1: BMS Voltage MIN
// 0: BMS Voltage MIN
struct CAN_MOB ams1_mob;

// CAN MOB from right sensorhub.
// data layout:
// 7: X
// 6: X
// 5: Wheelspeed Front Right
// 4: Wheelspeed Front Right
// 3: APPS2
// 2: APPS2
// 1: APPS1
// 0: APPS1
struct CAN_MOB shr_mob;

// CAN MOB from left sensorhub.
// data layout:
// 7: Steering Angle
// 6: Steering Angle
// 5: Wheelspeed Front Left
// 4: Wheelspeed Front Left
// 3: Brake Pressure Sensor Rear
// 2: Brake Pressure Sensor Rear
// 1: Brake Pressure Sensor Front
// 0: Brake Pressure Sensor Front
struct CAN_MOB shl_mob;

// CAN MOB from back sensorhub.
// data layout:
// 7: Cooling Temperature Rear 2
// 6: Cooling Temperature Rear 2
// 5: Cooling Temperature Rear 1
// 4: Cooling Temperature Rear 1
// 3: Cooling Temperature Front 2
// 2: Cooling Temperature Front 2
// 1: Cooling Temperature Front 1
// 0: Cooling Temperature Front 1
struct CAN_MOB shb_mob;

// CAN MOB from back sensorhub.
// data layout:
// 7: X
// 6: X
// 5: X
// 4: X
// 3: X
// 2: Shut Down Circuit Indicator DIC
// 1: Ready 2 Drive
// 0: TS_ON
struct CAN_MOB dic_mob;

// CAN MOB from Fusebox.
// data layout:
// 7: Fuse Read Out
// 6: Fuse Read Out
// 5: X
// 4: Shutdown Circuit Indicator Fusebox
// 3: Voltage LV Battery
// 2: Voltage LV Battery
// 1: Voltage Board
// 0: Voltage Board
struct CAN_MOB fusebox_mob;

// CAN MOB from SWC.
// data layout:
// 7: X
// 6: X
// 5: X
// 4: X
// 3: Push Button right
// 2: Push Button left
// 1: Rotary Encoder right
// 0: Rotary Encoder left
struct CAN_MOB swc_mob;

// CAN MOB 0 from DATALOGGER.
// data layout:
// 7: X
// 6: X
// 5: YAW
// 4: YAW
// 3: PITCH
// 2: PITCH
// 1: ROLL
// 0: ROLL
struct CAN_MOB logger0_mob;

// CAN MOB 1 from DATALOGGER.
// data layout:
// 7: GPS_SPEED
// 6: GPS_SPEED
// 5: VERTICAL ACCELERATION
// 4: VERTICAL ACCELERATION
// 3: LATERAL_ACCELERATION
// 2: LATERAL_ACCELERATION
// 1: INLINE_ACCELERATION
// 0: INLINE_ACCELERATION
struct CAN_MOB logger1_mob;

// CAN MOB 2 from DATALOGGER.
// data layout:
// 7: PRED_TIME
// 6: PRED_TIME
// 5: PRED_TIME
// 4: PRED_TIME
// 3: BEST_TIME
// 2: BEST_TIME
// 1: BEST_TIME
// 0: BEST_TIME
struct CAN_MOB logger2_mob;

// CAN MOB 3 from DATALOGGER.
// data layout:
// 7: X
// 6: ODOMETER
// 5: ODOMETER
// 4: LAP_TIME
// 3: LAP_TIME
// 2: LAP_TIME
// 1: LAP_TIME
// 0: LAP_NUMBER
struct CAN_MOB logger3_mob;

// CAN MOB 0 from Inverter0.
// data layout:
// 7: Voltage In
// 6: Voltage In
// 5: DUTY
// 4: DUTY
// 3: ERPM
// 2: ERPM
// 1: ERPM
// 0: ERPM
struct CAN_MOB inv00_mob;

// CAN MOB 1 from Inverter0.
// data layout:
// 7: 0xFF
// 6: 0xFF
// 5: 0xFF
// 4: Fault Code
// 3: Motor Temperature
// 2: Motor Temperature
// 1: MCU Temperature
// 0: MCU Temperature
struct CAN_MOB inv01_mob;

// CAN MOB 0 from Inverter1.
// data layout:
// 7: Voltage In
// 6: Voltage In
// 5: DUTY
// 4: DUTY
// 3: ERPM
// 2: ERPM
// 1: ERPM
// 0: ERPM
struct CAN_MOB inv10_mob;

// CAN MOB 1 from Inverter1.
// data layout:
// 7: 0xFF
// 6: 0xFF
// 5: 0xFF
// 4: Fault Code
// 3: Motor Temperature
// 2: Motor Temperature
// 1: MCU Temperature
// 0: MCU Temperature
struct CAN_MOB inv11_mob;



void init_mobs(){
	
	ams0_mob.mob_id = AMS_0_MOB_ID;
	ams0_mob.mob_idmask = 0xffff;
	ams0_mob.mob_number = 0;
	
	ams1_mob.mob_id = AMS_1_MOB_ID;
	ams1_mob.mob_idmask = 0xffff;
	ams1_mob.mob_number = 1;
	
	shr_mob.mob_id = SHR_MOB_ID;
	shr_mob.mob_idmask = 0xffff;
	shr_mob.mob_number = 2;
	
	shl_mob.mob_id = SHL_MOB_ID;
	shl_mob.mob_idmask = 0xffff;
	shl_mob.mob_number = 3;
	
	shb_mob.mob_id = SHB_MOB_ID;
	shb_mob.mob_idmask = 0xffff;
	shb_mob.mob_number = 4;
	
	dic_mob.mob_id = DIC_MOB_ID;
	dic_mob.mob_idmask = 0xffff;
	dic_mob.mob_number = 5;
	
	fusebox_mob.mob_id = FUSEBOX_MOB_ID;
	fusebox_mob.mob_idmask = 0xffff;
	fusebox_mob.mob_number = 6;
	
	swc_mob.mob_id = SWC_MOB_ID;
	swc_mob.mob_idmask = 0xffff;
	swc_mob.mob_number = 7;
	
	logger0_mob.mob_id = LOGGER_0_MOB_ID;
	logger0_mob.mob_idmask = 0xffff;
	logger0_mob.mob_number = 8;
	
	logger1_mob.mob_id = LOGGER_1_MOB_ID;
	logger1_mob.mob_idmask = 0xffff;
	logger1_mob.mob_number = 9;
	
	logger2_mob.mob_id = LOGGER_2_MOB_ID;
	logger2_mob.mob_idmask = 0xffff;
	logger2_mob.mob_number = 10;
	
	logger3_mob.mob_id = LOGGER_3_MOB_ID;
	logger3_mob.mob_idmask = 0xffff;
	logger3_mob.mob_number = 11;
	
	/*
	inv00_mob.mob_id = INV0_0_MOB_ID;
	inv00_mob.mob_idmask = 0xffff;
	inv00_mob.mob_number = 12;
	
	inv01_mob.mob_id = INV0_1_MOB_ID;
	inv01_mob.mob_idmask = 0xffff;
	inv01_mob.mob_number = 13;
	
	inv10_mob.mob_id = INV1_0_MOB_ID;
	inv10_mob.mob_idmask = 0xffff;
	inv10_mob.mob_number = 14;
	
	inv11_mob.mob_id = INV1_1_MOB_ID;
	inv11_mob.mob_idmask = 0xffff;
	inv11_mob.mob_number = 15;
	
	*/
	
}

void can_receive(){
	can_rx(&logger1_mob, mob_databytes[LOGGER1_DATA]);
	can_rx(&logger2_mob, mob_databytes[LOGGER2_DATA]);
	can_rx(&fusebox_mob, mob_databytes[FUSEBOX_DATA]);
	can_rx(&swc_mob, mob_databytes[SWC_DATA]);
	can_rx(&ams0_mob, mob_databytes[AMS1_DATA]);
	can_rx(&ams1_mob, mob_databytes[AMS1_DATA]);
	can_rx(&shr_mob, mob_databytes[SHB_DATA]);
	can_rx(&shl_mob, mob_databytes[SHB_DATA]);
	can_rx(&shb_mob, mob_databytes[SHB_DATA]);
}

void can_transmit(){
	can_tx(&dic_mob, mob_databytes[DIC_DATA]);
}

uint8_t get_dsp_mode(){
	return mob_databytes[SWC_DATA][DSP_MODE_BYTE];
}

uint8_t* get_mob_data(uint8_t mob){
	return mob_databytes[mob];
}

void can_put_data(){
	
	ts_voltage = mob_databytes[AMS0_DATA][0] | (mob_databytes[AMS0_DATA][1] << 8);
	ts_current = mob_databytes[AMS0_DATA][2] | (mob_databytes[AMS0_DATA][3] << 8);
	state_of_charge = mob_databytes[AMS0_DATA][4] | (mob_databytes[AMS0_DATA][5] << 8);
	ams_error = ((mob_databytes[AMS0_DATA][6]>>7) & 1);
	imd_error = ((mob_databytes[AMS0_DATA][6]>>6) & 1);
	can_ok = ((mob_databytes[AMS0_DATA][6]>>5) & 1);
	precharge_active = ((mob_databytes[AMS0_DATA][6]>>4) & 1);
	TS_RDY = ((mob_databytes[AMS0_DATA][6]>>3) & 1);
	
	bms_min_voltage = mob_databytes[AMS1_DATA][0] | (mob_databytes[AMS1_DATA][1] << 8);
	bms_max_voltage = mob_databytes[AMS1_DATA][2] | (mob_databytes[AMS1_DATA][3] << 8);
	bms_min_temp = (mob_databytes[AMS1_DATA][4] | (mob_databytes[AMS1_DATA][5] << 8))/100;
	bms_max_temp = (mob_databytes[AMS1_DATA][6] | (mob_databytes[AMS1_DATA][7] << 8))/100;
	
	APPS = mob_databytes[SHL_DATA][0] | (mob_databytes[SHL_DATA][1] << 8);
	
	cooling_1 = mob_databytes[SHB_DATA][0] | (mob_databytes[SHB_DATA][1] << 8);
	cooling_2 = mob_databytes[SHB_DATA][2] | (mob_databytes[SHB_DATA][3] << 8);
	//cooling_temp = (cooling_1 + cooling_2) / 2 ;		//Mittelwert aus cooling 1 und 2
	cooling_temp_deg = (float) ((0.128479*cooling_1) - (8.137044))*10; // NTC Kurve ist linearisiert im Bereich 80°-20°, außerhalb ungenauer
	
	TS_ON = (~PINA & (1 << PA0));
	Ready_2_Drive = ((~PINA & (1 << PA1)) >> PA1);

	battery_voltage = mob_databytes[FUSEBOX_DATA][2] | (mob_databytes[FUSEBOX_DATA][3] << 8);
	SDCIFB = mob_databytes[FUSEBOX_DATA][4];
	fuse_readout = mob_databytes[FUSEBOX_DATA][6] | (mob_databytes[FUSEBOX_DATA][7] << 8);
	
	mob_databytes[DIC_DATA][0] = TS_ON;
	mob_databytes[DIC_DATA][1] = Ready_2_Drive;
	mob_databytes[DIC_DATA][2] = SDCIDIC;
	
	
	
	
}


