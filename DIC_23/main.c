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

extern uint16_t RPM;
uint8_t LED_counter1;
uint8_t LED_counter2;
uint8_t LED_RPM;

extern uint8_t Akku_fan;
//extern uint8_t Cooling_fan;
uint8_t Button_Akku = 0;
//uint8_t Button_Cool = 0;
 uint8_t Rotary_right = 0;
 uint8_t Rotary_left = 0;

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
	uint8_t error_ams = 2;
	
	uint8_t LED = 0;
	uint8_t time_150ms = 0;
	
	uint8_t activate_ams = 0;
	unsigned long time_100ms = 0;
	
	sei();
	//this needs interrupts to be enabled
	configure_portextenders();
	pre_defined_led_colors_right(PE_OFF);
	pre_defined_led_colors_left(PE_OFF);
	//pre_defined_led_colors(PE_OFF);
	//bms_error(1);
	
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

	//Variable die das aktive Display h�lt!
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
			
			time_10ms = 0;
			time_50ms++;
			time_100ms++;
			time_150ms++;
			active_display = &dsp_main;
			can_receive();
			//get_mob_data(AMS2_DATA);
			can_put_data();
			can_transmit();
			if(led_test == 1)
			{
				bms_error(1);
				pre_defined_led_colors_right(PE_RED);
				pre_defined_led_colors_left(PE_RED);
				//pre_defined_led_colors(PE_RED);
				//extender_leds_blocking(RGB_LEFT,0|(1<<F_RED));
				PORTA |= (1<<PA2);
				
				//led_top_light(LED);
				//LED++;
				
				display_main(active_display);
				
				if(time_150ms > 14)
				{
					led_top_light(LED);
					LED++;
					time_150ms = 0;
				}
				
				
				//LED_RPM = (RPM/466,66) - 1
				//for (LED_counter1 = 0; LED_counter1 < LED_RPM;LED_counter1++)
				//{
				//	led_top_light(LED_counter1);
				//}
				
				for (LED_counter2 = LED_counter1 + 1; LED_counter2 < 15; LED_counter2++)
				{
					led_top_clear(LED_counter2);
				}
				
				
			}
		
			
			
			
		}
		if(time_100ms > 299)
		{
			
			//bms_error(0);
			for (uint8_t LED = 14; LED > 0; LED--)
			{
				led_top_clear(LED);
			}
			
			
			//if(Rotary_right == 0)
			//{
			//	display_main(active_display);
			//}
			//if(Rotary_right == 1)
			//{
			//	display_clear(active_display);
			//}
			
			
			
			led_test = 0;
			PORTA &= ~(1<<PA2);
			
			pre_defined_led_colors_right(PE_OFF);
			pre_defined_led_colors_left(PE_GREEN);
			
			if(Akku_fan == 1)
			{
				Button_Akku = 1 - Button_Akku;
			}
			
			if(Button_Akku == 1)
			{
				pre_defined_led_colors_right(PE_BLUE);
			}
			
			if(Button_Akku == 0)
			{
				pre_defined_led_colors_right(PE_OFF);
			}
			
			//if(Cooling_fan == 1)
			//{
				//Button_Cool = 1 - Button_Cool;
			//}
			//
			//if(Button_Cool == 1)
			//{
				//pre_defined_led_colors_left(PE_GREEN);
			//}
			//
			//if(Button_Cool == 0)
			//{
				//pre_defined_led_colors_left(PE_OFF);
			//}
			
			//pre_defined_led_colors(PE_BLUE);
			//led_left_top_bar_select(5);
			//led_right_top_bar_select(10);
			
		}
		
		
		
		
	}
	
	}
		
	
