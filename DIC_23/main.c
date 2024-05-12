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

extern uint16_t bms_max_voltage;
extern uint16_t bms_min_voltage;
extern uint16_t bms_max_temp;
extern uint16_t bms_min_temp;
extern uint16_t cooling_front;
extern uint16_t pressure_front;
extern uint16_t pressure_rear;


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
	
	struct DISPLAY_PAGE dsp_driver = get_empty_display();
	
	display_write_str(&dsp_startup, "   Baldig Resing    ",0,0);
	display_write_str(&dsp_startup, "        .--.        ",1,0);
	display_write_str(&dsp_startup, "   .----'   '--.    ",2,0);
	display_write_str(&dsp_startup, "   '-()-----()-'    ",3,0);
	
	display_write_str(&dsp_driver, "TSV:XXXV      AT:XXC",0,0);
	display_write_str(&dsp_driver, "LVV:XX.XV     CT:XXC",1,0);
	display_write_str(&dsp_driver, "BB:XX       CTMD:XXC",2,0);
	display_write_str(&dsp_driver, "ERR:      BP_F:XXbar",3,0);
	
	
	
	//Variable die das Aktive Display hält!
	struct DISPLAY_PAGE *active_display = &dsp_driver;
	
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
		
		if (time_10ms >=10){
			
			time_10ms = 0;
			time_50ms++;
			time_100ms++;
			can_receive();
			//get_mob_data(AMS2_DATA);
			can_put_data();
			can_transmit();
			//cooling Auswertung
			uint16_t tnp_temp = (float) ((0.128479*cooling_front) - (8.137044)); // NTC Kurve ist linearisiert im Bereich 80°-20°, außerhalb ungenauer
			
			uint16_t bf_pressure = (float) (pressure_front/4,5*100);
			uint16_t br_pressure = (float) (pressure_rear/4,5*100);
			
			
			dsp_driver.data[1][18] = tnp_temp%10+0x30;
			tnp_temp = tnp_temp/10;
			dsp_driver.data[1][17] = tnp_temp%10+0x30;
			
			dsp_driver.data[1][56] = bf_pressure%10+0x30;
			bf_pressure = bf_pressure/10;
			dsp_driver.data[1][55] = bf_pressure%10+0x30;
		}
				
}
		}
		
		//if (time_50ms > 4){
		//	display_large_number(&dsp_startup,0,get_mob_data(SWC_DATA)[0]);
		//	display_large_number(&dsp_startup,16,get_mob_data(SWC_DATA)[1]);
		//	time_50ms = 0;
			//switch(sys_time%7){
			//	case 0:
			//		pre_defined_led_colors(PE_OFF);
			//		break;
			//	case 1:
			//		pre_defined_led_colors(PE_RED);
			//		break;
			//	case 2:
			//		pre_defined_led_colors(PE_BLUE);
			//		break;
			//	case 3:
			//		pre_defined_led_colors(PE_GREEN);
			//		break;
			//	case 4:
			//		pre_defined_led_colors(PE_AMBER);
			//		break;
			//	case 5:
			//		pre_defined_led_colors(PE_PURPLE);
			//		break;
			//	case 6:
			//		pre_defined_led_colors(PE_WHITE);
			//		break;
			//}
		//}