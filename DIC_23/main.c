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
extern uint8_t ams_error;
extern uint8_t imd_error;
uint8_t led_test = 1;
//extern uint16_t bms_min_temp;

uint8_t LED = 0;
extern uint8_t LED_RPM;
uint8_t LED_counter1 = 0;
uint8_t LED_counter2 = 0;
extern uint32_t RPM;

extern uint8_t Akku_fan;
uint8_t Button_Akku = 0;

//extern uint8_t Cooling_fan;
//uint8_t Button_Cooling;

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
	uint8_t time_150ms = 0;
	uint8_t error_ams = 2;
	uint8_t activate_ams = 0;
	unsigned long time_3000ms = 0;
	
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
		if (draw_data)
		{
			draw_data = 0;
			draw_char(active_display,&display_state);
			
		}
		if((sys_time - sys_time_old) >= 1)
		{
			sys_time_old = sys_time;
			time_10ms++;
		}
		
		
		if (time_10ms > 9)
		{
			can_transmit();
			can_receive();
			can_put_data();
			//display_main(active_display);
			display_clear(active_display);
			
			if (led_test == 0)
			{
			
				//for (int i = 0; i < 15; i++)
				//{
					//if (i >= LED_RPM)
					//{
						//led_top_clear(i);  // Schalte die LED ein
					//}
					//else
					//{
						//led_top_light(i);  // Schalte die LED aus
					//}
				//}
				//LED_RPM = ((RPM/466.66) - 1);
				
				for (LED_counter2 = LED_RPM; LED_counter2 < 16; LED_counter2++)
				{
					led_top_clear(LED_counter2);
				}
				
				for (LED_counter1 = 1; LED_counter1 <= LED_RPM; LED_counter1++)
				{
					led_top_light(LED_counter1);
				}
				
			} //end if(led_test == 0)
			
			
			
			if(led_test == 1)
			{
				bms_error(1);
				PORTA |= (1<<PA2);
				
				pre_defined_led_colors_right(PE_RED);
				pre_defined_led_colors_left(PE_RED);
				
				if(time_150ms > 14)
				{
					led_top_light(LED);
					LED++;
					time_150ms = 0;
				} //end if(time_150ms > 14)
				time_150ms++;
				
				if(time_3000ms > 290)
				{
					for (LED = 0; LED < 16; LED++)
					{
						led_top_clear(LED);
					} //end for-Schleife
				} //end if(time_3000 > 250)
				
			 } //end if(led_test == 1)
			
			time_10ms = 0;
			time_3000ms++;
			
		} //end if(time_10ms > 9)
		
		if(time_3000ms > 299){
			active_display = &dsp_main;
			led_test = 0;
			PORTA &= ~(1<<PA2);
			
			pre_defined_led_colors_right(PE_OFF);
			pre_defined_led_colors_left(PE_OFF);
			
			//if (Akku_fan == 1)
			//{
				//Button_Akku = 1 - Button_Akku;
			//}
			//
			//if (Button_Akku == 1)
			//{
				//pre_defined_led_colors_right(PE_BLUE);
			//} 
			//else
			//{
				//pre_defined_led_colors_right(PE_OFF);
			//}
			
			
			//if (Cooling_fan == 1)
			//{
			//Button_Cooling = 1 - Button_Cooling;
			//}
			//
			//if (Button_Cooling == 1)
			//{
			//pre_defined_led_colors_left(PE_GREEN);
			//}
			//else
			//{
			//pre_defined_led_colors_left(PE_OFF);
			//}
			
			time_3000ms=0;
		} //end if(time_3000ms > 299)
	} //end while(1)
} //end main
		
	
