/*
 * display.h
 *
 * Created: 11.09.2022 13:59:37
 *  Author: Admin
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

struct DISPLAY_PAGE{
	uint8_t data[4][20];
};

struct DISPLAY_STATE{
	uint8_t row;
	uint8_t column;	
};

struct DISPLAY_STATE get_empty_state();
struct DISPLAY_PAGE get_empty_display();

void display_customchar(uint8_t charpos, uint8_t line1, uint8_t line2, uint8_t line3, uint8_t line4, uint8_t line5, uint8_t line6, uint8_t line7, uint8_t line8);
void display_definechars();

void draw_char(struct DISPLAY_PAGE *display,struct DISPLAY_STATE *display_state);

void init_display();

void display_write_data(uint8_t data,uint8_t rs);

void display_write_str(struct DISPLAY_PAGE *display, char data[], uint8_t row, uint8_t column);

void display_large_number(struct DISPLAY_PAGE *display, uint8_t offset, uint8_t number);
void display_small_number(struct DISPLAY_PAGE *display, uint8_t offset, uint8_t row, uint8_t number);

void display_digits(struct DISPLAY_PAGE *display, uint8_t digits, uint8_t offset, uint8_t row, uint16_t number, uint8_t comma);

void display_voltage(struct DISPLAY_PAGE *display, uint16_t number);

void display_temp(struct DISPLAY_PAGE *display, uint16_t number);

void display_main(struct DISPLAY_PAGE *display);

void display_meme(struct DISPLAY_PAGE *display);
	
#endif /* DISPLAY_H_ */