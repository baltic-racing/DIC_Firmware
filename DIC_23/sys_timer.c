/*
 * sys_timer.c
 *
 * Created: 23.12.2022 20:20:14
 *  Author: Ole Hannemann
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

volatile unsigned long sys_time = 0;
volatile uint8_t draw_data = 0;


void sys_timer_config(void)
{

	//8 bit Timer 0 configuration
	//Mode --> CTC, Divider --> /64
	TCCR0A = 0 | (1<<WGM01) | (1<<CS01) | (1<<CS00);
	//compare interrupt enable
	TIMSK0 = 0 | (1<<OCIE0A);
	//compare value for 1ms;
	OCR0A = 250;
}

/*	Interrupt Service Routines	*/
ISR(TIMER0_COMP_vect)
{
	sys_time++;
	draw_data++;
}