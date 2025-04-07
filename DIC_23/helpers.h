/*
 * helpers.h
 *
 * Created: 11.09.2022 14:17:58
 *  Author: Admin
 */ 


#ifndef HELPERS_H_
#define HELPERS_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

uint8_t invert_binary_number(uint8_t number_to_invert);
uint8_t gray2bin(uint8_t gray);




#endif /* HELPERS_H_ */