/*
 * candata.c
 *
 * Created: 23.12.2022 22:47:21
 *  Author: Ole Hannemann
 */ 

#include "candata.h"

uint8_t mob_databytes[12][8];

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

uint16_t APPS1 = 0;
uint16_t APPS2 = 0;
uint16_t BPSF = 0;
uint16_t BPSR = 0;
uint16_t cooling_1 = 0;
uint16_t cooling_2 = 0;
uint16_t cooling_temp = 0;
float cooling_temp_deg = 0;
uint16_t motor_temp = 0;
uint16_t mcu_temp = 0;

uint8_t Ready_2_Drive = 0;
uint8_t TS_ON = 0;
uint8_t SDCIDIC = 0;

uint16_t battery_voltage = 0;
uint8_t SDCIFB = 0;
uint16_t fuse_readout = 0;

uint16_t gps_speed = 0;

uint32_t ERPM_0 = 0;
uint32_t ERPM_1 = 0;

//uint16_t RPM = 0;

uint16_t mcu_temp_0 = 0;
uint16_t motor_temp_0 = 0;
uint8_t fault_code_0 = 0;

uint16_t mcu_temp_1 = 0;
uint16_t motor_temp_1 = 0;
uint8_t fault_code_1 = 0;

uint8_t ams_error_counter = 0;
uint8_t last_ams_counter = 0;

uint8_t Akku_fan = 0;
uint8_t Cooling_fan = 0;
extern uint8_t Rotary_right;
extern uint8_t Rotary_left;


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
// 5: Akku_fan_on Status
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
	
	
	inv00_mob.mob_id = INV0_0_MOB_ID;
	inv00_mob.mob_idmask = 0xffff;
	inv00_mob.mob_number = 8;
	
	inv01_mob.mob_id = INV0_1_MOB_ID;
	inv01_mob.mob_idmask = 0xffff;
	inv01_mob.mob_number = 9;
	
	inv10_mob.mob_id = INV0_1_MOB_ID;
	inv10_mob.mob_idmask = 0xffff;
	inv10_mob.mob_number = 10;
	
	inv11_mob.mob_id = INV1_1_MOB_ID;
	inv11_mob.mob_idmask = 0xffff;
	inv11_mob.mob_number = 11;
	
	
	
}

void can_receive(){
	
	can_rx(&ams0_mob, mob_databytes[AMS0_DATA]);
	uint8_t ams_counter = mob_databytes[AMS0_DATA][7];
	if (ams_counter == last_ams_counter){
		ams_error_counter++;
	} else {
		ams_error_counter = 0;
	}
	last_ams_counter = ams_counter;			// AMS error muss 100 mal kommen, dann dann leuchtet led bar 
	if (ams_error_counter > 100  || ams_error != 0){
		//LED anschalten
		bms_error(1);
	} else {
		bms_error(0);
	}
	
	can_rx(&ams1_mob, mob_databytes[AMS1_DATA]);
	can_rx(&shr_mob, mob_databytes[SHR_DATA]);
	can_rx(&shl_mob, mob_databytes[SHL_DATA]);
	can_rx(&shb_mob, mob_databytes[SHB_DATA]);
	can_rx(&fusebox_mob, mob_databytes[FUSEBOX_DATA]);
	can_rx(&swc_mob, mob_databytes[SWC_DATA]);
	can_rx(&inv01_mob, mob_databytes[INV01_DATA]);
	can_rx(&inv11_mob, mob_databytes[INV11_DATA]);
	
	
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
	
	ts_voltage = (mob_databytes[AMS0_DATA][0] | (mob_databytes[AMS0_DATA][1] << 8));
	ts_voltage = ts_voltage/100;
	//ts_current = mob_databytes[AMS0_DATA][2] | (mob_databytes[AMS0_DATA][3] << 8);
	//state_of_charge = mob_databytes[AMS0_DATA][4] | (mob_databytes[AMS0_DATA][5] << 8);
	ams_error = ((mob_databytes[AMS0_DATA][6]>>7) & 1);
	imd_error = ((mob_databytes[AMS0_DATA][6]>>6) & 1);
	if (imd_error == 0){
		//pre_defined_led_colors(PE_RED);
		PORTA |= (0<<PA2);
		//extender_leds_blocking(RGB_LEFT,0|(0<<F_RED));
	}
	else
	{
		//pre_defined_led_colors(PE_OFF);
		PORTA |= (1<<PA2);
		//extender_leds_blocking(RGB_LEFT,0|(1<<F_RED));
	}
	//can_ok = ((mob_databytes[AMS0_DATA][6]>>5) & 1);
	precharge_active = ((mob_databytes[AMS0_DATA][6]>>4) & 1);
    //TS_RDY = ((mob_databytes[AMS0_DATA][6]>>3) & 1);
	
	//bms_min_voltage = mob_databytes[AMS1_DATA][0] | (mob_databytes[AMS1_DATA][1] << 8);
	//bms_max_voltage = mob_databytes[AMS1_DATA][2] | (mob_databytes[AMS1_DATA][3] << 8);
	//bms_min_temp = (mob_databytes[AMS1_DATA][4] | (mob_databytes[AMS1_DATA][5] << 8))/100;
	bms_max_temp = (mob_databytes[AMS1_DATA][6] | (mob_databytes[AMS1_DATA][7] << 8))/100;
	
	APPS1 = mob_databytes[SHR_DATA][0] | (mob_databytes[SHR_DATA][1] << 8);
	APPS2 = mob_databytes[SHR_DATA][2] | (mob_databytes[SHR_DATA][3] << 8);
	BPSF = mob_databytes[SHL_DATA][0] | (mob_databytes[SHL_DATA][1] << 8);
	BPSR = mob_databytes[SHL_DATA][2] | (mob_databytes[SHL_DATA][3] << 8);
	
	Akku_fan = ((mob_databytes[FUSEBOX_DATA][5]>>7) & 1);
	//Cooling_fan = ((mob_databytes[FUSEBOX_DATA][5]>>6) & 1);
	Rotary_right = (mob_databytes[SWC_DATA][1]);
	Rotary_left = (mob_databytes[SWC_DATA][0]);
	
	//cooling_1 = (mob_databytes[SHB_DATA][0] | (mob_databytes[SHB_DATA][1] << 8));
	//cooling_2 = mob_databytes[SHB_DATA][2] | (mob_databytes[SHB_DATA][3] << 8);
	//cooling_temp = (cooling_1 + cooling_2) / 2 ;		//Mittelwert aus cooling 1 und 2
	//cooling_temp_deg =  ((0.128*cooling_1) - (8.137))*10; // NTC Kurve ist linearisiert im Bereich 80°-20°, außerhalb ungenauer
	
	TS_ON = (~PINA & (1 << PA0));
	Ready_2_Drive = 0;
	/*
	if (BPSR >= 10 && precharge_active){
		Ready_2_Drive = (~PINA >> PA1) & PA1;
	}
	*/
	if (BPSR >= 10){
		Ready_2_Drive = (~PINA >> PA1) & PA1;
	}
	
	battery_voltage = mob_databytes[FUSEBOX_DATA][2] | (mob_databytes[FUSEBOX_DATA][3] << 8);
	//SDCIFB = mob_databytes[FUSEBOX_DATA][4];
	//fuse_readout = mob_databytes[FUSEBOX_DATA][6] | (mob_databytes[FUSEBOX_DATA][7] << 8);
	
	
	ERPM_0 = (mob_databytes[INV00_DATA][3] | (mob_databytes[INV00_DATA][2] << 8) | (mob_databytes[INV00_DATA][1] << 16) | (mob_databytes[INV00_DATA][0] << 24));
	ERPM_1 = (mob_databytes[INV10_DATA][3] | (mob_databytes[INV10_DATA][2] << 8) | (mob_databytes[INV10_DATA][1] << 16) | (mob_databytes[INV10_DATA][0] << 24));
	
	//RPM = (ERPM_0 + ERPM_1)/2
	
	motor_temp_1 = (mob_databytes[INV01_DATA][3] | (mob_databytes[INV01_DATA][2] << 8));
	motor_temp_0 = (mob_databytes[INV11_DATA][3] | (mob_databytes[INV11_DATA][2] << 8));
	
	if (motor_temp_1>motor_temp_0)
	{
		motor_temp = motor_temp_1;
	} 
	else
	{
		motor_temp = motor_temp_0;
	}
	
	mcu_temp_0 = (mob_databytes[INV01_DATA][1] | (mob_databytes[INV01_DATA][0] << 8));
	mcu_temp_1 = (mob_databytes[INV11_DATA][1] | (mob_databytes[INV11_DATA][0] << 8));
	
	if (mcu_temp_1>mcu_temp_0)
	{
		mcu_temp = mcu_temp_1;
	}
	else
	{
		mcu_temp = mcu_temp_0;
	}
	mob_databytes[DIC_DATA][0] = TS_ON;
	mob_databytes[DIC_DATA][1] = Ready_2_Drive;
	mob_databytes[DIC_DATA][2] = SDCIDIC;
	
	
	
	
}


