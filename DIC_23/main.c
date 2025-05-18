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
extern uint16_t battery_voltage;
extern uint16_t cooling_1;
extern uint16_t APPS1;
extern uint16_t APPS2;
extern uint16_t BPSF;
extern uint16_t BPSR;
extern uint16_t motor_temp;
extern uint16_t mcu_temp;
extern uint16_t motor_temp_1;
extern uint16_t mcu_temp_1;
extern uint16_t motor_temp_0;
extern uint16_t mcu_temp_0;

// extern uint16_t bms_max_voltage;
//extern uint16_t bms_min_voltage;
uint8_t led_test = 1;
uint8_t system_startup = 0;
//extern uint16_t bms_min_temp;


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
	
	uint8_t dsp = 0;
	uint8_t time_10ms = 0;
	uint8_t time_50ms = 0;
	uint8_t time_100ms = 0;
	uint8_t error_ams = 2;
	uint8_t activate_ams = 0;
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
	display_write_str(&dsp_startup, "|      TY 25       |",1,0);
	display_write_str(&dsp_startup, "|    BEWARE OF     |",2,0);
	display_write_str(&dsp_startup, "|   HIGH TORQUE    |",3,0);
	
	//display_write_str(&dsp_startup, "|                  |",0,0);
	//display_write_str(&dsp_startup, "|      .kotz       |",1,0);
	//display_write_str(&dsp_startup, "|                  |",2,0);
	//display_write_str(&dsp_startup, "|                  |",3,0);
	
	
	 

	struct DISPLAY_PAGE dsp_debug = get_empty_display();
	//dsp_debug.number = 2;
	display_write_str(&dsp_debug,"TSV:   V  ACCU:  . C", 0, 0);
	display_write_str(&dsp_debug,"LVV:  . V COOL:  . C", 1, 0);
	display_write_str(&dsp_debug,"APPS1:   %APPS2:   %", 2, 0);
	display_write_str(&dsp_debug,"BPF:  BPR:          ", 3, 0);
	
	struct DISPLAY_PAGE dsp_temp = get_empty_display();
	
	struct DISPLAY_PAGE dsp_main = get_empty_display();
	
	
	display_write_str(&dsp_main,"INV:  . C MTR:   . C", 0, 0);
	display_write_str(&dsp_main,"TSV:   V  ACCU:  . C", 1, 0);
	display_write_str(&dsp_main,"LVV:  . V COOL:  . C", 2, 0);
	display_write_str(&dsp_main,"APPS:               ", 3, 0);
	

	//Variable die das aktive Display hält!
	struct DISPLAY_PAGE *active_display = &dsp_startup;
	
	uint32_t page_order[3] = {
		&dsp_startup,
		&dsp_main,
		&dsp_debug
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
				dsp_main.data [0][18] = (motor_temp%10)+48;			// 48 ist der ascii offset
				dsp_main.data [0][16] = ((motor_temp/10)%10)+48;
				dsp_main.data [0][15] = ((motor_temp/100)%10)+48;
				
				dsp_main.data [0][7] = (mcu_temp%10)+48;
				dsp_main.data [0][5] = ((mcu_temp/10)%10)+48;
				dsp_main.data [0][4] = ((mcu_temp/100)%10)+48;
				
				dsp_main.data [1][6] = (ts_voltage%10)+48;
				dsp_main.data [1][5] = ((ts_voltage/10)%10)+48;
				dsp_main.data [1][4] = ((ts_voltage/100)%10)+48;
				
				dsp_main.data [1][18] = (bms_max_temp%10)+48;
				dsp_main.data [1][16] = ((bms_max_temp/10)%10)+48;
				dsp_main.data [1][15] = ((bms_max_temp/100)%10)+48;
				
				dsp_main.data [2][7] = (battery_voltage%10)+48;
				dsp_main.data [2][5] = ((battery_voltage/10)%10)+48;
				dsp_main.data [2][4] = ((battery_voltage/100)%10)+48;
				
				dsp_main.data [2][18] = (cooling_1%10)+48;
				dsp_main.data [2][16] = ((cooling_1/10)%10)+48;
				dsp_main.data [2][15] = ((cooling_1/100)%10)+48;
				
				dsp_main.data [3][8] = (APPS1%10)+48;
				dsp_main.data [3][7] = ((APPS1/10)%10)+48;
				dsp_main.data [3][6] = ((APPS1/100)%10)+48;
				
			}
			if(active_display == &dsp_debug)
			{
				dsp_debug.data [0][6] = (ts_voltage%10)+48;
				dsp_debug.data [0][5] = ((ts_voltage/10)%10)+48;
				dsp_debug.data [0][4] = ((ts_voltage/100)%10)+48;
				
				dsp_debug.data [0][18] = (bms_max_temp%10)+48;
				dsp_debug.data [0][16] = ((bms_max_temp/10)%10)+48;
				dsp_debug.data [0][15] = ((bms_max_temp/100)%10)+48;
				
				dsp_debug.data [1][7] = (battery_voltage%10)+48;
				dsp_debug.data [1][5] = ((battery_voltage/10)%10)+48;
				dsp_debug.data [1][4] = ((battery_voltage/100)%10)+48;
				
				dsp_debug.data [1][18] = (cooling_1%10)+48;
				dsp_debug.data [1][16] = ((cooling_1/10)%10)+48;
				dsp_debug.data [1][15] = ((cooling_1/100)%10)+48;
				
				dsp_debug.data [2][8] = (APPS1%10)+48;
				dsp_debug.data [2][7] = ((APPS1/10)%10)+48;
				dsp_debug.data [2][6] = ((APPS1/100)%10)+48;
				
				dsp_debug.data [2][18] = (APPS2%10)+48;
				dsp_debug.data [2][17] = ((APPS2/10)%10)+48;
				dsp_debug.data [2][16] = ((APPS2/100)%10)+48;
				
				dsp_debug.data [3][5] = (BPSF%10)+48;
				dsp_debug.data [3][4] = ((BPSF/10)%10)+48;
				
				dsp_debug.data [3][11] = (BPSR%10)+48;
				dsp_debug.data [3][10] = ((BPSR/10)%10)+48;
			}
			
			//else
			//{
				//display_write_str(active_display, "       HIER         ",0,0);
				//display_write_str(active_display, "      KÖNNTEN       ",1,0);
				//display_write_str(active_display, "     IHRE DATEN     ",2,0);
				//display_write_str(active_display, "       STEHEN       ",3,0);
			//}
			
			if(led_test){
				bms_error(1);
				PORTA |= (1<<PA2);		// IMD LED
				if (sys_time >= LED_TEST_TIME){	// IMD & AMS error LED test 
					PORTA &= ~(1<<PA2);
					led_test = 0;
				}
			}
			
			
			PORTG ^= (1<<PG3);		//Heart LED
			time_10ms = 0;
			time_300ms++;
			
			if(sys_time >= SYSTEM_STARTUP_TIME){
				
				active_display = page_order[get_dsp_mode()%NELEMS(page_order)];
			}
			
			
		}
		if (time_100ms > 99){
			
			can_transmit();
			
			time_100ms = 0;
		}
		
	}
}
		
	
