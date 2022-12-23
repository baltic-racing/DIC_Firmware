/*
 * port_definitions.c
 *
 * Created: 11.09.2022 14:13:34
 *  Author: Admin
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "port_definitions.h"


void port_config(){
	DDRE = 0b11111111;
	DDRB = 0 | (1<<PB6) | (1<<PB5)| (1<<PB4) | (1<<PB3);
}