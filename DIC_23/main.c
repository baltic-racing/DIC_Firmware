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

// extern uint16_t bms_max_voltage;
//extern uint16_t bms_min_voltage;
extern uint16_t bms_max_temp;
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
	unsigned long time_100ms = 0;
	
	sei();
	//this needs interrupts to be enabled
	configure_portextenders();
	pre_defined_led_colors(PE_OFF);
	
	//dispaly state -> saves postitions and stuff
	struct DISPLAY_STATE display_state = get_empty_state();
	
	//display pages holding data
	struct DISPLAY_PAGE dsp_startup = get_empty_display();
	
	struct DISPLAY_PAGE dsp_voltage = get_empty_display();
	
	struct DISPLAY_PAGE dsp_temp = get_empty_display();
	
	struct DISPLAY_PAGE dsp_main = get_empty_display();
	

	
	display_write_str(&dsp_startup, "   Baldig Resing    ",0,0);
	display_write_str(&dsp_startup, "        .--.        ",1,0);
	display_write_str(&dsp_startup, "   .----'   '--.    ",2,0);
	display_write_str(&dsp_startup, "   '-()-----()-'    ",3,0);

	//Variable die das aktive Display hält!
	struct DISPLAY_PAGE *active_display = &dsp_startup;
	
	while (1)
	{
		if (draw_data){
			draw_data = 0;
			draw_char(active_display,&display_state);
			
		}
		if((sys_time - sys_time_old) >= 1){
			sys_time_old = sys_time;
			time_10ms++;
		}
		
		if (time_10ms > 9){
			
			time_10ms = 0;
			time_50ms++;
			time_100ms++;
			can_receive();
			//get_mob_data(AMS2_DATA);
			can_put_data();
			can_transmit();
		}
		if(time_100ms > 299){
			active_display = &dsp_main;
			
			display_main(active_display );
			pre_defined_led_colors(PE_AMBER);
			led_left_top_bar_select(5);
			led_right_top_bar_select(10);
			bms_error(1);
			
		}
		
	}
	
	}
		
	
