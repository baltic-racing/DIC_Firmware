/*
 * portextender.c
 *
 * Created: 23.12.2022 14:09:48
 *  Author: Ole Hannemann
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "portextender.h"

//some global vars for out interrupt routine and to store data
volatile uint8_t spi_state = 0;
volatile struct SPI_MSG current_msg;


// The following function configures the AT90CAN spi hardware
void configure_spi_bus(){
	
	//Lest bitte das nächste mal das Datenblatt...
	//Kapitel 16.2.2 vom AT90CAN Datenblatt
	DDRB |= (1<<PB0);
	
	// configure MOSI and SCK as Output
	//PB1 - SCK
	//PB2 - MOSI
	DDRB |= (1<<PB2)|(1<<PB1);
	//Enable SPI, SPI Interrupts and select Master Mode
	SPCR = 0|(1<<SPE)|(1<<SPIE)|(1<<MSTR);
	//SET CLOCK PRESCALER to / 16
	//CLK ~ 1MHz
	SPCR |= (1<<SPR0);
	
	//mark CS Pins as Output
	DDRA |= (1<<PA3);
	DDRB |= (1<<PB7);
	//THE CS ARE ACTIVE LOW -> PULLING THEM HIGH!
	DIS_CS_LEFT
	DIS_CS_LEFT
		
}

// the following function checks if we completed our SPI Data Transfer
// 1 means busy
// 0 mens we are done!
uint8_t spi_free(){
	
	if(spi_state == 0){
		return 1;
	}
	return 0;
	
}

// address is for the 2 bytes of possible Address pins
void send_data(enum portextender extender, uint8_t control, uint8_t reg, uint8_t data){
	
	current_msg.selected_extender = extender;
	current_msg.register_addr = reg;
	current_msg.control_byte = control;
	current_msg.data = data;
	
	spi_state = 1;
	
	//set chip select!
	
	//Write the control byte into the SPI Data Register
	switch(extender){
		case RGB_LEFT:
			EN_CS_LEFT
			//make sure chip select for the right is off
			DIS_CS_RIGHT
			break;
		case RGB_RIGHT:
			EN_CS_RIGHT
			//make sure chip select for the left is off
			DIS_CS_LEFT
			break;
	}
	SPDR = control;
	
};

void send_data_blocking(enum portextender extender, uint8_t control, uint8_t reg, uint8_t data){
	//wait until the bus is free
	while (!spi_free()){
		//wait for some time
		WASTE_CPU_TIME
	}
	send_data(extender,control,reg,data);
};



void extender_leds(enum portextender extender, uint8_t data){
	
	send_data(extender,0b01000000,PE_OLAT,~data);
};

void extender_leds_blocking(enum portextender extender, uint8_t data){
	send_data_blocking(extender,0b01000000,PE_OLAT,~data);
}

ISR(SPI_STC_vect){
	switch (spi_state){
		case 1:
			//control byte was send!
			//send register
			SPDR = current_msg.register_addr;
			//update SPI State			
			spi_state = 2;
			break;
		case 2:
			//register was send
			//send data
			SPDR = current_msg.data;
			//update SPI State
			spi_state = 3;
			break;
		case 3:
			//data was send
			//disable chip selects
			DIS_CS_RIGHT
			DIS_CS_LEFT
			//set spi state to 0
			spi_state = 0;
			break;
	}
	
}

void configure_portextenders(){
	
	//mark all pins as output
	send_data_blocking(RGB_LEFT,0b01000100,PE_IODIR,0x00);
	send_data_blocking(RGB_RIGHT,0b01000010,PE_IODIR,0x00);

	//disable HA Pins...
	send_data_blocking(RGB_LEFT,0b01000100,PE_IOCON,0);
	send_data_blocking(RGB_RIGHT,0b01000010,PE_IOCON,0);
}

void pre_defined_led_colors(enum led_color color)
{
	switch(color)
	{
		case PE_RED:
		extender_leds_blocking(RGB_LEFT,0|(1<<F_RED)|(1<<R_RED));
		extender_leds_blocking(RGB_RIGHT,0|(1<<F_RED)|(1<<R_RED));
		break;
		case PE_GREEN:
		extender_leds_blocking(RGB_LEFT,0|(1<<F_GREEN)|(1<<R_GREEN));
		extender_leds_blocking(RGB_RIGHT,0|(1<<F_GREEN)|(1<<R_GREEN));
		break;
		case PE_BLUE:
		extender_leds_blocking(RGB_LEFT,0|(1<<F_BLUE)|(1<<R_BLUE));
		extender_leds_blocking(RGB_RIGHT,0|(1<<F_BLUE)|(1<<R_BLUE));
		break;
		case PE_PURPLE:
		extender_leds_blocking(RGB_LEFT,0|(1<<F_BLUE)|(1<<R_BLUE)|(1<<R_RED)|(1<<F_RED));
		extender_leds_blocking(RGB_RIGHT,0|(1<<F_BLUE)|(1<<R_BLUE)|(1<<R_RED)|(1<<F_RED));
		break;
		case PE_LIGHTBLUE:
		extender_leds_blocking(RGB_LEFT,0|(1<<F_GREEN)|(1<<R_GREEN)|(1<<R_BLUE)|(1<<F_BLUE));
		extender_leds_blocking(RGB_RIGHT,0|(1<<F_GREEN)|(1<<R_GREEN)|(1<<R_BLUE)|(1<<F_BLUE));
		break;
		case PE_WHITE:
		extender_leds_blocking(RGB_LEFT,0xff);
		extender_leds_blocking(RGB_RIGHT,0xff);
		break;
		case PE_OFF:
		extender_leds_blocking(RGB_LEFT,0);
		extender_leds_blocking(RGB_RIGHT,0);
		break;
		case PE_AMBER:
		extender_leds_blocking(RGB_LEFT,0|(1<<F_GREEN)|(1<<R_GREEN)|(1<<R_RED)|(1<<F_RED));
		extender_leds_blocking(RGB_RIGHT,0|(1<<F_GREEN)|(1<<R_GREEN)|(1<<R_RED)|(1<<F_RED));
		break;
	}

}


void pre_defined_led_colors_right(enum led_color color)
{
	switch(color)
	{
		case PE_RED:
		extender_leds_blocking(RGB_RIGHT,0|(1<<F_RED)|(1<<R_RED));
		break;
		case PE_GREEN:
		extender_leds_blocking(RGB_RIGHT,0|(1<<F_GREEN)|(1<<R_GREEN));
		break;
		case PE_BLUE:
		extender_leds_blocking(RGB_RIGHT,0|(1<<F_BLUE)|(1<<R_BLUE));
		break;
		case PE_PURPLE:
		extender_leds_blocking(RGB_RIGHT,0|(1<<F_BLUE)|(1<<R_BLUE)|(1<<R_RED)|(1<<F_RED));
		break;
		case PE_LIGHTBLUE:
		extender_leds_blocking(RGB_RIGHT,0|(1<<F_GREEN)|(1<<R_GREEN)|(1<<R_BLUE)|(1<<F_BLUE));
		break;
		case PE_WHITE:
		extender_leds_blocking(RGB_RIGHT,0xff);
		break;
		case PE_OFF:
		extender_leds_blocking(RGB_RIGHT,0);
		break;
		case PE_AMBER:
		extender_leds_blocking(RGB_RIGHT,0|(1<<F_GREEN)|(1<<R_GREEN)|(1<<R_RED)|(1<<F_RED));
		break;
	}
}




void pre_defined_led_colors_left(enum led_color color)
{
	switch(color)
	{
		case PE_RED:
		extender_leds_blocking(RGB_LEFT,0|(1<<F_RED)|(1<<R_RED));
		break;
		case PE_GREEN:
		extender_leds_blocking(RGB_LEFT,0|(1<<F_GREEN)|(1<<R_GREEN));
		break;
		case PE_BLUE:
		extender_leds_blocking(RGB_LEFT,0|(1<<F_BLUE)|(1<<R_BLUE));
		break;
		case PE_PURPLE:
		extender_leds_blocking(RGB_LEFT,0|(1<<F_BLUE)|(1<<R_BLUE)|(1<<R_RED)|(1<<F_RED));
		break;
		case PE_LIGHTBLUE:
		extender_leds_blocking(RGB_LEFT,0|(1<<F_GREEN)|(1<<R_GREEN)|(1<<R_BLUE)|(1<<F_BLUE));
		break;
		case PE_WHITE:
		extender_leds_blocking(RGB_LEFT,0xff);
		break;
		case PE_OFF:
		extender_leds_blocking(RGB_LEFT,0);
		break;
		case PE_AMBER:
		extender_leds_blocking(RGB_LEFT,0|(1<<F_GREEN)|(1<<R_GREEN)|(1<<R_RED)|(1<<F_RED));
		break;
	}
}





//This Function turns off the left Top Bar
void clear_top_left_bar(void)
{
	PORTA &= ~(1<<PA4) & ~(1<<PA5) & ~(1<<PA6) & ~(1<<PA7);
	PORTG &= ~(1<<PG2);
}

//This Function turns off the right Top Bar
void clear_top_right_bar(void)
{
	PORTC = 0;
	PORTG &= ~(1<<PG0) & ~(1<<PG1);
}

//This Function calculates the amount of LEDs of the left Top Bar which needs to be turned on
void led_top_left_bar(uint16_t max_value_l, uint16_t min_value_l, uint16_t current_value_l)
{
	clear_top_left_bar();
	uint16_t Top_Left_Bar_Divider = (max_value_l - min_value_l) / (LED_COUNT_TOP_LEFT - 1);
	int8_t amount_l = (current_value_l - min_value_l + Top_Left_Bar_Divider) / Top_Left_Bar_Divider;
	if (amount_l > 0) {led_left_top_bar_select(amount_l);}
	else {led_left_top_bar_select(0);}
}

//This Function calculates the amount of LEDs of the right Top Bar which needs to be turned on
void led_top_right_bar(uint16_t max_value_r, uint16_t min_value_r, uint16_t current_value_r)
{
	clear_top_right_bar();
	uint16_t Top_Right_Bar_Divider = (max_value_r - min_value_r) / (LED_COUNT_TOP_RIGHT - 1);
	int8_t amount_r = (current_value_r - min_value_r + Top_Right_Bar_Divider) / Top_Right_Bar_Divider;
	if (amount_r > 0) {led_right_top_bar_select(amount_r);}
	else {led_right_top_bar_select(0);}
}

//This Function turns on the desired amount of LEDs of the left Top Bar
void led_left_top_bar_select(uint8_t select_l)
{
	PORTA |= ~(0xFF << select_l) << PA4;
	PORTG |= (1<<PG2) & ((0x80) >> select_l);
}

//This Function turns on the desired amount of LEDs of the right Top Bar
void led_right_top_bar_select(uint8_t select_r)
{
	PORTC |= ~(0xFF >> select_r);
	PORTG |= (~(0xFF << (select_r/9)) << (select_r/9)) + (select_r/10);
}

void led_top_light( uint8_t LED)
{
	
		switch(LED){
			
			case 0: {PORTA |= (1<<PA4); break;}
			case 1: {PORTA |= (1<<PA5); break;}	
			case 2: {PORTA |= (1<<PA6); break;}
			case 3: {PORTA |= (1<<PA7); break;}
			case 4: {PORTG |= (1<<PG2); break;}
			case 5: {PORTC |= (1<<PC7); break;}
			case 6: {PORTC |= (1<<PC6); break;}
			case 7: {PORTC |= (1<<PC5); break;}
			case 8: {PORTC |= (1<<PC4); break;}
			case 9: {PORTC |= (1<<PC3); break;}
			case 10: {PORTC |= (1<<PC2); break;}
			case 11: {PORTC |= (1<<PC1); break;}
			case 12: {PORTC |= (1<<PC0); break;}
			case 13: {PORTG |= (1<<PG1); break;}
			case 14: {PORTG |= (1<<PG0); break;}
		}

	
}

void led_top_clear( uint8_t LED)
{
	
	switch(LED){
		
		case 0: {PORTA &= (1<<PA4); break;}
		case 1: {PORTA &= (1<<PA5); break;}
		case 2: {PORTA &= (1<<PA6); break;}
		case 3: {PORTA &= (1<<PA7); break;}
		case 4: {PORTG &= (1<<PG2); break;}
		case 5: {PORTC &= (1<<PC7); break;}
		case 6: {PORTC &= (1<<PC6); break;}
		case 7: {PORTC &= (1<<PC5); break;}
		case 8: {PORTC &= (1<<PC4); break;}
		case 9: {PORTC &= (1<<PC3); break;}
		case 10: {PORTC &= (1<<PC2); break;}
		case 11: {PORTC &= (1<<PC1); break;}
		case 12: {PORTC &= (1<<PC0); break;}
		case 13: {PORTG &= (1<<PG1); break;}
		case 14: {PORTG &= (1<<PG0); break;}
		
	}

	
}

void bms_error(uint8_t error)
{
	switch (error)
	{
		case 0: {PORTD &= (0b01100000); break;}	//Every 1 in this number represents an Pin which is connected to the bottom LED bar, 0 = on
		case 1: {PORTD |= (0b10011111); break;}	//Every 1 in this number represents an Pin which is connected to the bottom LED bar, 1 = on
	}
}

void led_startup_animation(uint8_t frame)
{
	
			led_top_light(7+frame);
			led_top_light(7-frame);
			
				

			if(frame >= 8)
			{
				pre_defined_led_colors(PE_GREEN);
			}
			
			if(frame >= 9)
			{
				bms_error(1);
			} 
			
			if(frame >= 14)
			{
				pre_defined_led_colors(PE_OFF);
				bms_error(0);
			}
			
			
			
			
			
		/*
			if (frame >= 2)
				{
					led_top_clear(7 + (frame - 2));
					led_top_clear(7 - (frame + 2));
				}
				*/
		
}

