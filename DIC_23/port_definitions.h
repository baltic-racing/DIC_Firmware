/*
 * port_definitions.h
 *
 * Created: 11.09.2022 14:05:34
 *  Author: Admin
 */ 


#ifndef PORT_DEFINITIONS_H_
#define PORT_DEFINITIONS_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>


#define SET_RS_PIN PORTB |= (1<<PB6);
#define UNSET_RS_PIN PORTB &= ~(1<<PB6);

#define SET_ENABLE_PIN PORTB |= (1<<PB4);
#define UNSET_ENABLE_PIN PORTB &= ~(1<<PB4);

#define DISPLAY_DATA PORTE

void port_config();




#endif /* PORT_DEFINITIONS_H_ */