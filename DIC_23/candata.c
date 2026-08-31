/*
 * candata.c
 *
 * Created: 23.12.2022 22:47:21
 *  Author: Ole Hannemann
 */ 



#include "candata.h"
#include "canlib.h"
#include "portextender.h"
#include "display.h"
#include "helpers.h"
#include "sys_timer.h"


#define BSPD_STARTUP_TIME 15000
#define AMS_DISCONNECT_TIME 1000


uint8_t mob_databytes[12][8];

uint16_t ts_voltage = 0;
uint16_t ts_current = 0;
uint16_t state_of_charge = 0;
uint8_t ams_error = 0;
uint8_t imd_error = 0;
uint8_t can_ok = 0;
uint8_t precharge_active = 0;
uint8_t TS_RDY = 0;

uint8_t bms_error_active = 0;
uint8_t ams_error_counter1 = 0;

uint16_t bms_max_voltage = 0;
uint16_t bms_min_voltage = 0;
uint16_t bms_max_temp = 0;
uint16_t bms_min_temp = 0;


uint16_t APPS1 = 0;
uint16_t APPS2 = 0;
uint16_t BPSF = 0;
uint16_t BPSR = 0;
uint16_t brake_balance_front = 0;
uint16_t brake_balance_rear = 0;
uint16_t brake_sum = 0;

int8_t steering_angle = 0;
uint8_t SAS_abs = 0;
char SAS_sign = ' ';

uint16_t motor_temp = 0;
uint16_t mcu_temp = 0; 

uint8_t Ready_2_Drive = 0;
uint8_t TS_ON = 0;
uint8_t TSon_enabled =0;

uint16_t battery_voltage = 0;

uint16_t fuse_readout = 0;
uint8_t FRO_value = 0; 
uint8_t fr_values[14] = {0};

//uint8_t Akku_fan_status = 0;

uint16_t ERPM_0 = 0;
uint16_t ERPM_1 = 0;

uint16_t mcu_temp_0 = 0;
uint16_t motor_temp_0 = 209;
uint8_t fault_code_0 = 0;

uint16_t mcu_temp_1 = 0;
uint16_t motor_temp_1 = 0;
uint8_t fault_code_1 = 0;

uint16_t ams_error_counter = 0;
uint8_t last_ams_counter = 0;

extern volatile unsigned long sys_time;
volatile unsigned long ams_disconnect_timestamp = 0;
extern uint8_t wait;
extern uint8_t active_mode;

static uint8_t imd_error_count = 0;
static uint8_t IMD_LED_ON = 0;
#define IMD_ERROR_THRESHOLD 200  // Anzahl ben?tigter Empfangsvorg?nge



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

// CAN MOB from right sensorhub. //0x400
// data layout:
// 7: SAS
// 6: X
// 5: X
// 4: X
// 3: DTS_FL
// 2: DTS_FL
// 1: DTS_FR
// 0: DTS_FR
struct CAN_MOB shr_mob;

// CAN MOB from SH front. //0x401
// data layout:
// 7: X
// 6: X
// 5: X
// 4: X
// 3: Brake Pressure Sensor Rear
// 2: Brake Pressure Sensor Rear
// 1: Brake Pressure Sensor Front
// 0: Brake Pressure Sensor Front
struct CAN_MOB shf_mob;

// CAN MOB from Sensorhub back //0x420 //unused ID
// data layout:
// 7: X
// 6: X
// 5: X
// 4: X
// 3: X
// 2: X
// 1: X
// 0: X
struct CAN_MOB shb_mob;

// CAN MOB from DIC.
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
// 7: FuseReadOut
// 6: FuseReadOut
// 5: SDC_status (high)
// 4: SDC-status (low)
// 3: LV-Voltage_mV (high)
// 2: LV-Voltage_mV (low)
// 1: Bat_sense_mV	(high)
// 0: Bat_sense_mV  (low) 
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

// CAN MOB from VCU.
// data layout:
// 7: X
// 6: X
// 5: X
// 4: start_motor_control
// 3: APPS2
// 2: APPS2
// 1: APPS1
// 0: APPS1
struct CAN_MOB vcu_mob;

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


void init_mobs(){
	
	ams0_mob.mob_id = AMS_0_MOB_ID; //0x200
	ams0_mob.mob_idmask = 0xffff;
	ams0_mob.mob_number = 0;
	
	ams1_mob.mob_id = AMS_1_MOB_ID; //0x201
	ams1_mob.mob_idmask = 0xffff;
	ams1_mob.mob_number = 1;
	
	shr_mob.mob_id = SHR_MOB_ID; //0x400
	shr_mob.mob_idmask = 0xffff;
	shr_mob.mob_number = 2;
	
	shf_mob.mob_id = SHF_MOB_ID; //0x401
	shf_mob.mob_idmask = 0xffff;
	shf_mob.mob_number = 3;
	
	shb_mob.mob_id = SHB_MOB_ID; //0x420
	shb_mob.mob_idmask = 0xffff;
	shb_mob.mob_number = 4;
	
	dic_mob.mob_id = DIC_MOB_ID; //0x500
	dic_mob.mob_idmask = 0xffff;
	dic_mob.mob_number = 5;
	
	fusebox_mob.mob_id = FUSEBOX_MOB_ID; //0x600
	fusebox_mob.mob_idmask = 0xffff;
	fusebox_mob.mob_number = 6;
	
	swc_mob.mob_id = SWC_MOB_ID; //0x750
	swc_mob.mob_idmask = 0xffff;
	swc_mob.mob_number = 7;
	
	vcu_mob.mob_id = vcu_0_mob_ID; //0x300
	vcu_mob.mob_idmask = 0xffff;
	vcu_mob.mob_number = 8;
		
	inv00_mob.mob_id = INV0_0_MOB_ID; //0x44A
	inv00_mob.mob_idmask = 0xffff;
	inv00_mob.mob_number = 9;
	
	inv01_mob.mob_id = INV0_1_MOB_ID; //0x453
	inv01_mob.mob_idmask = 0xffff;
	inv01_mob.mob_number = 10;
	
}
void check_bms_imd_status(void)
{
	// - - - ams master freeze - - -
	uint8_t ams_counter = mob_databytes[AMS0_DATA][7];
	if (ams_counter != last_ams_counter)
	{
		ams_disconnect_timestamp = sys_time;
	}
	last_ams_counter = ams_counter;

	if (sys_time - ams_disconnect_timestamp >= AMS_DISCONNECT_TIME)
	{
		bms_error(1);
	}

	// - - - ams error send by master - - - 
	ams_error = ((mob_databytes[AMS0_DATA][6] >> 7) & 1);

	if (ams_error == 1 && !bms_error_active)
	{
		ams_error_counter1++;
	}
	else if (ams_error == 0)
	{
		ams_error_counter1 = 0;
	}

	if (ams_error_counter1 >= 50)
	{
		bms_error(1);
		bms_error_active = 1;
	}

	// - - - imd error - -  -
	imd_error = ((mob_databytes[AMS0_DATA][6] >> 6) & 1);

	if (imd_error == 0 && !IMD_LED_ON)
	{
		imd_error_count = 0;
		PORTA &= ~(1 << PA2);
	}
	else if (!IMD_LED_ON)
	{
		if (imd_error_count < IMD_ERROR_THRESHOLD)
		{
			imd_error_count++;
		}

		if (imd_error_count >= IMD_ERROR_THRESHOLD)
		{
			IMD_LED_ON = 1;
			PORTA |= (1 << PA2);
		}
	}
}
void can_receive(){
	
	can_rx(&ams0_mob, mob_databytes[AMS0_DATA]);
	can_rx(&ams1_mob, mob_databytes[AMS1_DATA]);
	can_rx(&shr_mob, mob_databytes[SHR_DATA]);
	can_rx(&shf_mob, mob_databytes[SHF_DATA]);
	can_rx(&shb_mob, mob_databytes[SHB_DATA]);
	can_rx(&fusebox_mob, mob_databytes[FUSEBOX_DATA]);
	can_rx(&swc_mob, mob_databytes[SWC_DATA]);
	can_rx(&vcu_mob, mob_databytes[VCU_DATA]);
	can_rx(&inv00_mob, mob_databytes[INV00_DATA]);
	can_rx(&inv01_mob, mob_databytes[INV01_DATA]);
		
}

void can_transmit(){
	can_tx(&dic_mob, mob_databytes[DIC_DATA]);
}

uint8_t get_dsp_mode(){
	return mob_databytes[SWC_DATA][DSP_MODE_BYTE];
	
}
uint8_t get_swc_buttons(){
	/*
	 gibt die Knöpfe des Lenkrads als ein zusammengesetztes int8_t 
	 ---x PBL Push button Left
	 --x- PBR Push button Right
	 -x-- SWTL Schaltwippe top left
	 x--- SWTR Schaltwippe top right
	*/
	uint8_t wert = 0;
	
	wert = mob_databytes[SWC_DATA][PBL] | (mob_databytes[SWC_DATA][PBR]<<1) | (((~mob_databytes[SWC_DATA][SWTL])&1)<<2) | (((~mob_databytes[SWC_DATA][SWTR])&1)<<3);
	return wert;
}

uint8_t* get_mob_data(uint8_t mob){
	return mob_databytes[mob];
}

void can_put_data(){
	
	ts_voltage = (mob_databytes[AMS0_DATA][0] | (mob_databytes[AMS0_DATA][1] << 8));
	ts_voltage = ts_voltage/100;

	ams_error = ((mob_databytes[AMS0_DATA][6]>>7) & 1);
	imd_error = ((mob_databytes[AMS0_DATA][6]>>6) & 1);
	
	precharge_active = ((mob_databytes[AMS0_DATA][6]>>4) & 1);	

	bms_max_temp = (mob_databytes[AMS1_DATA][6] | (mob_databytes[AMS1_DATA][7] << 8));
	bms_max_temp = bms_max_temp/100;
	
	APPS1 = (mob_databytes[VCU_DATA][0] | (mob_databytes[VCU_DATA][1] << 8))/10;
	APPS2 = (mob_databytes[VCU_DATA][2] | (mob_databytes[VCU_DATA][3] << 8))/10;

	BPSF = (mob_databytes[SHF_DATA][0] | (mob_databytes[SHF_DATA][1] << 8))/10;

	BPSR = (mob_databytes[SHF_DATA][2] | (mob_databytes[SHF_DATA][3] << 8))/10;

	brake_sum = (4*BPSF) + (2*BPSR);		//*4 and *2 for break pistons
	if (brake_sum > 10)
	{
		brake_balance_front = (uint16_t)((4*BPSF * 100UL) / brake_sum);
		brake_balance_rear = (uint16_t)((2*BPSR * 100UL) / brake_sum);
	}

	steering_angle = (int8_t)mob_databytes[SHR_DATA][7];

	SAS_abs = (steering_angle < 0) ? (uint8_t)(-steering_angle) : (uint8_t)steering_angle;
	SAS_sign = (steering_angle < 0) ? '-' : ' ';
	
	battery_voltage = mob_databytes[FUSEBOX_DATA][2] | (mob_databytes[FUSEBOX_DATA][3] << 8);
	
	motor_temp_1 = (mob_databytes[INV00_DATA][3] | (mob_databytes[INV00_DATA][2] << 8));
	motor_temp_0 = (mob_databytes[INV01_DATA][3] | (mob_databytes[INV01_DATA][2] << 8));
	
	if (motor_temp_1>motor_temp_0)
	{
		motor_temp = motor_temp_1;
	} 
	else
	{
		motor_temp = motor_temp_0;
	}
	
	mcu_temp_0 = (mob_databytes[INV00_DATA][1] | (mob_databytes[INV00_DATA][0] << 8));		//INV Temp
	mcu_temp_1 = (mob_databytes[INV01_DATA][1] | (mob_databytes[INV01_DATA][0] << 8));		//INV Temp
	
	if (mcu_temp_1>mcu_temp_0)
	{
		mcu_temp = mcu_temp_1;
	}
	else
	{
		mcu_temp = mcu_temp_0;
	}

	if(sys_time>=BSPD_STARTUP_TIME && wait == 0)
	{
		TS_ON = (~PINA & (1 << PA0));
		
		if(TSon_enabled==0)
		{
			pre_defined_led_colors(PE_OFF);
		}
		TSon_enabled=1;
	}


	if (!(PINA & (1 << PA1)))
	{
		Ready_2_Drive = 1;
	}
	else
	{
		Ready_2_Drive = 0;
	}

	mob_databytes[DIC_DATA][0] = TS_ON;
	mob_databytes[DIC_DATA][1] = Ready_2_Drive;
	mob_databytes[DIC_DATA][2] = 0;
	mob_databytes[DIC_DATA][5] = 0;
	mob_databytes[DIC_DATA][7] = active_mode;
	
	
	
		
}


