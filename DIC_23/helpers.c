/*
 * helpers.c
 *
 * Created: 11.09.2022 14:17:47
 *  Author: Admin
 */ 

#include "helpers.h"

uint8_t invert_binary_number(uint8_t number_to_invert){
	
	uint8_t out_num = 0;
	
	for (uint8_t it = 0 ; it < 8; it++){

		out_num |= ((number_to_invert>> it) & 1)<<(7-it);
		
	}
	return out_num;
	
	
}
uint8_t gray2bin(uint8_t gray){
	
	int bin;
	int i;
	for(i=5; i >= 0; i--){
		bin = bin | ((gray & 1<<i)>>i ^ (bin & 1<<(i + 1))>>i)<<i;
	}
	return bin;
}

