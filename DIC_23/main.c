/*	INCLUDES	*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include "canlib.h"
#include "candata.h"
#include "display.h"
#include "port_definitions.h"
#include "portextender.h"
#include "sys_timer.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))


#define LED_TEST_TIME 3000			// Duration of LED test in ms
#define SYSTEM_STARTUP_TIME 4000	// Duration of system startup in ms


extern uint16_t ts_voltage;
extern uint16_t bms_max_temp;

extern uint16_t APPS1;
extern uint16_t APPS2;
extern uint16_t BPSF;
extern uint16_t BPSR;

extern char SAS_sign;
extern uint8_t SAS_abs;

extern uint16_t brake_balance_front;
extern uint16_t brake_balance_rear;

extern uint16_t battery_voltage;
extern uint8_t FRO_value;
extern uint8_t fr_values[14];

extern uint16_t motor_temp;
extern uint16_t mcu_temp;
extern uint16_t motor_temp_1;
extern uint16_t mcu_temp_1;
extern uint16_t motor_temp_0;
extern uint16_t mcu_temp_0;

//extern uint8_t Akku_fan_status;
//void Akku_fan_LED(Akku_fan_status);

uint8_t led_test = 1;
uint8_t system_startup = 0;
uint8_t wait = 0;

uint8_t mode_selection_active = 0;
uint8_t active_mode = 3;
uint8_t selected_mode = 3;



/*	MAIN	*/
int main(void)
{
	port_config();
	init_display();
	display_definechars();
	configure_spi_bus();
	sys_timer_config();
	init_mobs();
	can_cfg();
	
	extern volatile unsigned long sys_time;
	extern volatile uint8_t draw_data;
	
	volatile unsigned long sys_time_old;
	
	uint8_t time_10ms = 0;
	uint8_t time_100ms = 0;
	unsigned long time_300ms = 0;
	
	sei();
	//this needs interrupts to be enabled
	configure_portextenders();
	pre_defined_led_colors(PE_RED);
	
	
	//dispaly state -> saves postitions and stuff
	struct DISPLAY_STATE display_state = get_empty_state();
	
	//display pages holding data
	struct DISPLAY_PAGE dsp_startup = get_empty_display();
	//dsp_startup.number = 0;
	display_write_str(&dsp_startup, "|    WELCOME TO    |",0,0);
	display_write_str(&dsp_startup, "|      TY 26       |",1,0);
<<<<<<< Updated upstream
	display_write_str(&dsp_startup, "|    BEWARE OF     |",2,0);
	display_write_str(&dsp_startup, "| Martin Mondarsch |",3,0);
=======
	display_write_str(&dsp_startup, "|    TOD DEM DEM   |",2,0);
	display_write_str(&dsp_startup, "|     KORMORAN     |",3,0);
>>>>>>> Stashed changes
	

	struct DISPLAY_PAGE dsp_debug = get_empty_display();
	//dsp_debug.number = 2;
	display_write_str(&dsp_debug,"APPS1:       BPF:   ", 0, 0);
	display_write_str(&dsp_debug,"APPS2:       BPR:   ", 1, 0);
	display_write_str(&dsp_debug,"Balance:     F :   %", 2, 0);
	display_write_str(&dsp_debug,"             R :   %", 3, 0);
	
	struct DISPLAY_PAGE dsp_main = get_empty_display();
	display_write_str(&dsp_main,"INV :  . C APPS:    ", 0, 0);
	display_write_str(&dsp_main,"MTR :  . C BP  :    ", 1, 0);
	display_write_str(&dsp_main,"ACCU:  . C SAS :    ", 2, 0);
	display_write_str(&dsp_main,"LVV :  . V TSV :   V", 3, 0);
	
	/*
	struct DISPLAY_PAGE dsp_FRO0 = get_empty_display();
	//
	display_write_str(&dsp_FRO0,"---FUSE READ OUT----", 0, 0);
	display_write_str(&dsp_FRO0,"AIM :               ", 1, 0);
	display_write_str(&dsp_FRO0,"INV1:      INV0:    ", 2, 0);
	display_write_str(&dsp_FRO0,"R2D2:      WP  :    ", 3, 0);
	
	struct DISPLAY_PAGE dsp_FRO1 = get_empty_display();
	//
	display_write_str(&dsp_FRO1,"BL  :      SERV:    ", 0, 0);
	display_write_str(&dsp_FRO1,"SDC :      TSAL:    ", 1, 0);
	display_write_str(&dsp_FRO1,"TSAC:      HV_D:    ", 2, 0);
	display_write_str(&dsp_FRO1,"FTSA:      FPU  :   ", 3, 0);
	*/

	//SDC Indicator anzeige
	struct DISPLAY_PAGE dsp_SDCI = get_empty_display();
	
	display_write_str(&dsp_SDCI,"B_DB:      DIST:    ", 0, 0);
	display_write_str(&dsp_SDCI,"INER:      TSAC:    ", 1, 0);
	display_write_str(&dsp_SDCI,"BOTS:      B_R :    ", 2, 0);
	display_write_str(&dsp_SDCI,"B_L :      TSMS:    ", 3, 0);
	
	struct DISPLAY_PAGE dsp_reduced = get_empty_display();
	//dsp_debug.number = 2;
	display_write_str(&dsp_reduced,"TSV :       V       ", 0, 0);
	display_write_str(&dsp_reduced,"                    ", 1, 0);
	display_write_str(&dsp_reduced,"ACCU:    .  C       ", 2, 0);
	display_write_str(&dsp_reduced,"                    ", 3, 0);
	
	struct DISPLAY_PAGE dsp_modes = get_empty_display();
	//dsp_debug.number = 2;
	display_write_str(&dsp_modes,"--------SET---------", 0, 0);
	display_write_str(&dsp_modes,"MODE:     ENDURANCE ", 1, 0);
	display_write_str(&dsp_modes,"                    ", 2, 0);
	display_write_str(&dsp_modes,"                    ", 3, 0);
	

	//Variable die das aktive Display hält!
	struct DISPLAY_PAGE *active_display = &dsp_startup;
	
	uint32_t page_order[6] = {
		&dsp_startup,
		&dsp_main,
		&dsp_debug,
		&dsp_SDCI,
		&dsp_reduced,
		&dsp_modes
		
		/*&dsp_FRO0,
		&dsp_FRO1,*/
		//&dsp_SDC0,
		//&dsp_SDCI1
	};
	
	active_display = &dsp_startup;
	
	while (1)
	{
		if (draw_data){
			draw_data = 0;
			draw_char(active_display,&display_state);
			
		}
		if((sys_time - sys_time_old) >= 1){
			sys_time_old = sys_time;
			time_10ms++;
			time_100ms++;
		}
		
		
		if (time_10ms >9){
			
			
			
			
			
			can_receive();
			can_put_data();
			
			
			if(active_display == &dsp_startup)
			{
				//pre_defined_led_colors(PE_WHITE);
				//led_top_light(0);
				//led_top_light(1);
				//led_top_light(2);
				//led_top_light(3);
				//led_top_light(4);
				//led_top_light(5);
				//led_top_light(6);
				//led_top_light(7);
				//led_top_light(8);
				//led_top_light(9);
				//led_top_light(10);
				//led_top_light(11);
				//led_top_light(12);
				//led_top_light(13);
				//led_top_light(14);
				
			}
			
			if(active_display == &dsp_main)
			{
				dsp_main.data [0][8] = (mcu_temp%10)+48;
				dsp_main.data [0][6] = ((mcu_temp/10)%10)+48;
				dsp_main.data [0][5] = ((mcu_temp/100)%10)+48;

				dsp_main.data [0][19] = (APPS1%10)+48;
				dsp_main.data [0][18] = ((APPS1/10)%10)+48;
				dsp_main.data [0][17] = ((APPS1/100)%10)+48;


				dsp_main.data [1][8] = (motor_temp%10)+48;
				dsp_main.data [1][6] = ((motor_temp/10)%10)+48;
				dsp_main.data [1][5] = ((motor_temp/100)%10)+48;

				dsp_main.data [1][19] = (BPSF%10)+48;
				dsp_main.data [1][18] = ((BPSF/10)%10)+48;

				dsp_main.data [2][8] = (bms_max_temp%10)+48;
				dsp_main.data [2][6] = ((bms_max_temp/10)%10)+48;
				dsp_main.data [2][5] = ((bms_max_temp/100)%10)+48;

				dsp_main.data [2][19] = ((SAS_abs/1)%10)+48;
				dsp_main.data [2][18] = ((SAS_abs/10)%10)+48;
				dsp_main.data [2][17] = SAS_sign;

				dsp_main.data [3][8] = (battery_voltage%10)+48;
				dsp_main.data [3][6] = ((battery_voltage/10)%10)+48;
				dsp_main.data [3][5] = ((battery_voltage/100)%10)+48;

				dsp_main.data [3][18] = (ts_voltage%10)+48;
				dsp_main.data [3][17] = ((ts_voltage/10)%10)+48;
				dsp_main.data [3][16] = ((ts_voltage/100)%10)+48;
			}
			if(active_display == &dsp_debug)
			{
				
				dsp_debug.data [0][8] = (APPS1%10)+48;
				dsp_debug.data [0][7] = ((APPS1/10)%10)+48;
				dsp_debug.data [0][6] = ((APPS1/100)%10)+48;
				
				dsp_debug.data [1][8] = (APPS2%10)+48;
				dsp_debug.data [1][7] = ((APPS2/10)%10)+48;
				dsp_debug.data [1][6] = ((APPS2/100)%10)+48;
				
				dsp_debug.data [0][18] = (BPSF%10)+48;
				dsp_debug.data [0][17] = ((BPSF/10)%10)+48;
				
				dsp_debug.data [1][18] = (BPSR%10)+48;
				dsp_debug.data [1][17] = ((BPSR/10)%10)+48;

				dsp_debug.data [2][18] = (brake_balance_front%10)+48;
				dsp_debug.data [2][17] = ((brake_balance_front/10)%10)+48;
				dsp_debug.data [2][16] = ((brake_balance_front/100)%10)+48;

				dsp_debug.data [3][18] = (brake_balance_rear%10)+48;
				dsp_debug.data [3][17] = ((brake_balance_rear/10)%10)+48;
				dsp_debug.data [3][16] = ((brake_balance_rear/100)%10)+48;

}
			
			/*
			if (active_display == &dsp_FRO0)
			{
				dsp_FRO0.data [1][6] = ((fr_values[12]) & 1) ? ' ' : 'X';
				
				dsp_FRO0.data [2][6] = ((fr_values[11]) & 1) ? ' ' : 'X';
				
				dsp_FRO0.data [1][17] = ((fr_values[10]) & 1) ? ' ' : 'X';
				
				dsp_FRO0.data [3][6] = ((fr_values[9]) & 1) ? ' ' : 'X';
				
				dsp_FRO0.data [3][17] = ((fr_values[8]) & 1) ? ' ' : 'X';
			}
			
			if (active_display == &dsp_FRO1)
			{
				dsp_FRO1.data [0][6] = ((fr_values[7]) & 1) ? ' ' : 'X';
				
				dsp_FRO1.data [0][17] = ((fr_values[6]) & 1) ? ' ' : 'X';
				
				dsp_FRO1.data [1][6] = ((fr_values[5]) & 1) ? ' ' : 'X';
				
				dsp_FRO1.data [1][17] = ((fr_values[4]) & 1) ? ' ' : 'X';
			
				dsp_FRO1.data [2][6] = ((fr_values[3]) & 1) ? ' ' : 'X';
				
				dsp_FRO1.data [2][17] = ((fr_values[2]) & 1) ? ' ' : 'X';
				
				dsp_FRO1.data [3][6] = ((fr_values[1]) & 1) ? ' ' : 'X';
				
				dsp_FRO1.data [3][17] = ((fr_values[0]) & 1) ? ' ' : 'X';
			}
			*/

			if (active_display == &dsp_SDCI)
			{
				uint16_t sdc_status_dic = get_mob_data(FUSEBOX_DATA)[4] | (get_mob_data(FUSEBOX_DATA)[5] << 8);
				
				dsp_SDCI.data [0][6]  = ((sdc_status_dic >> 15) & 1) ? 'H' : ' ';   // B_DB
				dsp_SDCI.data [1][6]  = ((sdc_status_dic >> 14) & 1) ? 'H' : ' ';   // INER
				dsp_SDCI.data [2][6]  = ((sdc_status_dic >> 13) & 1) ? 'H' : ' ';   // BOTS
				dsp_SDCI.data [3][6]  = ((sdc_status_dic >> 12) & 1) ? 'H' : ' ';   // B_L
				dsp_SDCI.data [0][17] = ((sdc_status_dic >> 11) & 1) ? 'H' : ' ';   // DIST
				dsp_SDCI.data [1][17] = ((sdc_status_dic >> 8)  & 1) ? 'H' : ' ';   // TSAC
				dsp_SDCI.data [2][17] = ((sdc_status_dic >> 7)  & 1) ? 'H' : ' ';   // B_R
				dsp_SDCI.data [3][17] = ((sdc_status_dic >> 6)  & 1) ? 'H' : ' ';   // TSMS
			}	
			
			if (active_display == &dsp_reduced)
			{
				dsp_reduced.data [0][9] = (ts_voltage%10)+48;
				dsp_reduced.data [0][8] = ((ts_voltage/10)%10)+48;
				dsp_reduced.data [0][7] = ((ts_voltage/100)%10)+48;
				
				dsp_reduced.data [2][10] = (bms_max_temp%10)+48;
				dsp_reduced.data [2][8] = ((bms_max_temp/10)%10)+48;
				dsp_reduced.data [2][7] = ((bms_max_temp/100)%10)+48;	
			}
			
			if (active_display == &dsp_modes)
			{
				if(get_swc_buttons()==2){
					
					pre_defined_led_colors(PE_AMBER);
					mode_selection_active = 1;
					wait = 1;
					
				}
				if(mode_selection_active == 1 ){
					selected_mode = get_dsp_mode()%6;
					
					if (get_swc_buttons() == 1 ){
						
						active_mode = selected_mode;
						pre_defined_led_colors(PE_GREEN);
						mode_selection_active = 0;
						
					}
					
				}
				if (get_swc_buttons() == 12){
					mode_selection_active = 0;
					wait = 0;
					pre_defined_led_colors(PE_OFF);
					selected_mode = active_mode;
				}
				
				switch (selected_mode)
				{
					case 0: 
					display_write_str(&dsp_modes,"MODE:     ACCEL     ", 1, 0);
					break;
					case 1:
					display_write_str(&dsp_modes,"MODE:     SKIDPAD   ", 1, 0);
					break;
					case 2: 
					display_write_str(&dsp_modes,"MODE:     AUTOCROSS ", 1, 0);
					break;
					case 3: 
					display_write_str(&dsp_modes,"MODE:     ENDURANCE ", 1, 0);
					break;
					case 4: 
					display_write_str(&dsp_modes,"MODE:     FUN1      ", 1, 0);
					break;
					case 5: display_write_str(&dsp_modes,"MODE:     FUN2      ", 1, 0);
					break;
					
				}
			}
			
			
			if(led_test){
				bms_error(1);
				PORTA |= (1<<PA2);		// IMD LED
				if (sys_time >= LED_TEST_TIME){	// IMD & AMS error LED test 
					PORTA &= ~(1<<PA2);
					led_test = 0;
				}
<<<<<<< Updated upstream
=======

>>>>>>> Stashed changes
			}
<<<<<<< Updated upstream
			
<<<<<<< Updated upstream
			
=======
		
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
			PORTG ^= (1<<PG3);		//Heart LED
			time_10ms = 0;
			time_300ms++;
			
			if(sys_time >= SYSTEM_STARTUP_TIME)
			{
				if(wait == 0)
				{
					active_display = page_order[get_dsp_mode()%NELEMS(page_order)];
				}
				
				/*
				void Akku_fan_LED(Akku_fan_status)
				{
					switch (Akku_fan_status)
					{
						case 1:
						pre_defined_led_colors_right(PE_BLUE);
						break;
						case 0:
						pre_defined_led_colors_right(PE_OFF);
						break;
					}
				}*/
			}
			
			
		}
		if (time_100ms > 99){
			
			can_transmit();
			
			time_100ms = 0;
		}
		
	}
}
		
	
