/*
 * display.c
 *
 * Created: 11.09.2022 13:59:25
 *  Author: Admin
 */ 


#include "display.h"
#include "helpers.h"
#include "port_definitions.h"
#include "candata.h"

extern uint16_t ts_voltage;
extern uint16_t bms_max_temp;
extern uint16_t battery_voltage;
extern uint16_t cooling_1;
extern uint16_t APPS1;
extern uint16_t APPS2;
extern uint16_t BPSF;
extern uint16_t BPSR;
extern uint16_t motor_temp; 
extern uint16_t mcu_temp;
extern uint16_t motor_temp_1;
extern uint16_t mcu_temp_1;
extern uint16_t motor_temp_0;
extern uint16_t mcu_temp_0;


uint8_t dsp_command [7] = {
	0x39, //function set european chararacter set
	0x08, //display off
	0x06, //entry mode set increment cursor by 1 not shifting display
	0x17, //Character mode and internal power on
	0x00, //0x01, //clear display NEVER USE CLS IT WILL GET THE DISPLAY HOT SICNE EXECUTION TIME >2 ms
	0x02, //return home
	0x0C, //display on
};

uint8_t dsp_row[4] ={
	//This order is known by Trial and Error
	0xC0, //row 4
	0x94, //row 2
	0xD4, //row 3
	0x80, //row 1
};

void draw_char(struct DISPLAY_PAGE *display,struct DISPLAY_STATE *display_state){
	
	if (display_state->column == 20){
		display_write_data(dsp_row[display_state->row],0);
		display_state->column = 0;
		display_state->row += 1;
	}
	if (display_state->row == 4){
		display_state->row = 0;
	}
	display_write_data(display->data[display_state->row][display_state->column],1);
	display_state->column++;
}
struct DISPLAY_STATE get_empty_state(){
	struct DISPLAY_STATE display_state;
	display_state.row = 0;
	display_state.column = 0;
	
	return display_state;
}
struct DISPLAY_PAGE get_empty_display(){
	
	struct DISPLAY_PAGE display_data;
	
	for(uint8_t row = 0; row < 4; row++){
		for (uint8_t column = 0; column < 20; column++){
			display_data.data[row][column] = 0x10;
		}	 
	}
	return display_data;
}

void display_write_data(uint8_t data,uint8_t rs){
	
	DISPLAY_DATA = invert_binary_number(data);
	
	if (rs==1){
		SET_RS_PIN
		} else {
		UNSET_RS_PIN
	}
	
	//Sets enable high
	SET_ENABLE_PIN
	//Sets enable low
	for(int wait = 0; wait<100; wait++){
		UNSET_ENABLE_PIN
	}
	
	
}


//num_to_3digit this function not only converts the raw data to a 3Digit number which can be displayed onto the display, it also sets the Page and Position of the Number on the Display


void display_definechars(){
	//definition of needed custom chars
	display_customchar(1,0x1F,0x1F,0,0,0,0,0,0);
	display_customchar(2,0xEF,0xEF,0xEF,0xEF,0xEF,0xEF,0xEF,0xEF);
	display_customchar(3,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E);
	display_customchar(4,0xFF,0xFF,0xFF,0xFF,0,0,0,0);
	display_customchar(5,0xFF,0xFF,0xFF,0xFF,0xEF,0xEF,0xEF,0xEF);
	display_customchar(6,0x08,0x15,0x0A,0x04,0x0A,0x15,0x02,0x00);
	display_customchar(7,0,0,0,0,0,0x1F,0x1F,0x1F);
	
}

void display_customchar(uint8_t charpos, uint8_t line1, uint8_t line2, uint8_t line3, uint8_t line4, uint8_t line5, uint8_t line6, uint8_t line7, uint8_t line8){
	
	//where to write in cgram
	display_write_data(0x40+8*charpos,0);

	//write each line
	display_write_data(line1,1);
	display_write_data(line2,1);
	display_write_data(line3,1);
	display_write_data(line4,1);
	display_write_data(line5,1);
	display_write_data(line6,1);
	display_write_data(line7,1);
	display_write_data(line8,1);

	//display return home
	display_write_data(0x02,0);
}

void display_write_str(struct DISPLAY_PAGE *display, char data[], uint8_t row, uint8_t column)
{
	//Schreiben
	//Notiz < length is wichtig da so das 0 byte ignoriert wird
	//mit dem Zeichenketten terminiert werden
	uint8_t i = 0;
	while(data[i]!= 0 && (column+i)<20){
		display->data[row][i+column] = data[i];
		i++;
	}
}

void init_display(){
	
	//init the display with commands in dsp_command array
	for (int i=0; i<7; i++){
		display_write_data(dsp_command[i],0);
	}	
}

void display_main(struct DISPLAY_PAGE *display)
{
	uint8_t TS_ON = 0;
	uint8_t Ready_2_Drive = 0;
	
	TS_ON = (~PINA & (1 << PA0));
	Ready_2_Drive = ((~PINA & (1 << PA1)) >> PA1);
	display_write_str(display,"INV:  . C MTR:   . C", 0, 0);
	display_write_str(display,"TSV:   V  ACCU:  . C", 1, 0);
	display_write_str(display,"LVV:  . V COOL:25.0C", 2, 0);
	display_write_str(display,"APPS:               ", 3, 0);
	
	display_small_number(display,18, 0, motor_temp%10) ;
	display_small_number(display,16, 0, (motor_temp/10)%10);
	display_small_number(display,15, 0, (motor_temp/100)%10);
		
	display_small_number(display,7, 0, mcu_temp%10);
	display_small_number(display,5, 0, (mcu_temp/10)%10);
	display_small_number(display,4, 0, (mcu_temp/100)%10);
	
	display_small_number(display,6, 1,ts_voltage%10);
	display_small_number(display,5, 1, (ts_voltage/10)%10);
	display_small_number(display,4, 1, (ts_voltage/100)%10);
	
	display_small_number(display,18, 1,bms_max_temp%10);
	display_small_number(display,16, 1, (bms_max_temp/10)%10);
	display_small_number(display,15, 1, (bms_max_temp/100)%10);
	
	display_small_number(display,7, 2,battery_voltage%10) ;
	display_small_number(display,5, 2, (battery_voltage/10)%10);
	display_small_number(display,4, 2, (battery_voltage/100)%10);
	
	display_small_number(display,6, 3,APPS2%10) ;
	display_small_number(display,5, 3, (APPS2/10)%10);
	
	//display_small_number(display,18, 2, cooling_1%10) ;
	//display_small_number(display,16, 2, (cooling_1/10)%10);
	//display_small_number(display,15, 2, (cooling_1/100)%10);
	
}
void display_debug(struct DISPLAY_PAGE *display)
{
	uint8_t TS_ON = 0;
	uint8_t Ready_2_Drive = 0;
	
	TS_ON = (~PINA & (1 << PA0));
	Ready_2_Drive = ((~PINA & (1 << PA1)) >> PA1);
	display_write_str(display,"TSV:   V  ACCU:  . C", 0, 0);
	display_write_str(display,"LVV:  . V COOL:  . C", 1, 0);
	display_write_str(display,"APPS1:  %  APPS2:  %", 2, 0);
	display_write_str(display,"BPF:  BPR:          ", 3, 0);
	
	display_small_number(display,6, 0,ts_voltage%10) ;
	display_small_number(display,5, 0, (ts_voltage/10)%10);
	display_small_number(display,4, 0, (ts_voltage/100)%10);
	
	display_small_number(display,18, 0,bms_max_temp%10) ;
	display_small_number(display,16, 0, (bms_max_temp/10)%10);
	display_small_number(display,15, 0, (bms_max_temp/100)%10);
	
	display_small_number(display,7, 1,battery_voltage%10) ;
	display_small_number(display,5, 1, (battery_voltage/10)%10);
	display_small_number(display,4, 1, (battery_voltage/100)%10);
	
	display_small_number(display,18, 1, cooling_1%10) ;
	display_small_number(display,16, 1, (cooling_1/10)%10);
	display_small_number(display,15, 1, (cooling_1/100)%10);
	
	display_small_number(display,7, 2, APPS1%10);
	display_small_number(display,6, 2, (APPS1/10)%10);

	display_small_number(display,16, 2, APPS2%10);
	display_small_number(display,15, 2, (APPS2/10)%10);
	
	
	display_small_number(display,5, 3, (BPSF/10)%10);
	display_small_number(display,4, 3, (BPSF/100)%10);
	
	
	display_small_number(display,11, 3, (BPSR/10)%10);
	display_small_number(display,10, 3, (BPSR/100)%10);

}
void display_meme(struct DISPLAY_PAGE *display)
{
	 display_definechars();
}

void display_voltage(struct DISPLAY_PAGE *display, uint16_t number){
	//display_large_number(display, 0, 12);
	//display_write_str(display,"o",1,4);
	//display_write_str(display,"o",2,4);
	display_large_number(display, 14, 11);
	display_write_str(display, "\x07", 3, 4);
	display_large_number(display, 10, number%10);
	number = number/10;
	display_large_number(display, 6, number%10);
	number = number/10;
	display_large_number(display, 0, number%10);
}



void display_temp(struct DISPLAY_PAGE *display, uint16_t number){
	//display_large_number(display, 0, 12);
	//display_write_str(display,"o",1,4);
	//display_write_str(display,"o",2,4);
	display_large_number(display, 16, 13);
	display_write_str(display,"o", 0, 14);
	display_write_str(display, "\x07", 3, 8);
	display_large_number(display, 10, number%10);
	number = number/10;
	display_large_number(display, 4, number%10);
	number = number/10;
	display_large_number(display, 0, number%10);
}


// erzeugt beliebig langen string aus einem int wert, länge ist mit digits zu übergeben, geht nur ohne komma . comma gibt die Anzahl der nachkommastellen an
void display_digits(struct DISPLAY_PAGE *display, uint8_t digits, uint8_t offset,uint8_t row,  uint16_t number, uint8_t comma){
	
	uint8_t i;
	uint8_t comma_set = 0;
	
	for(i=0; i>= digits; i++)
	{
		
		
		if (comma >= i)
		{
			if (comma == i)
			{
				display_write_str(display, ",",row, (offset + digits-i));
				comma_set = 1;
			}
			
		}
		else
		{
			display_small_number(display,(offset + (digits-i-comma_set)), row, number%10);
			number = number/10;
		}
		
	}
}

	

void display_large_number(struct DISPLAY_PAGE *display, uint8_t offset, uint8_t number){

	switch (number){
		case 0:
			display_write_str(display, "\xff\xff\xff",0,offset);
			display_write_str(display, "\xff \xff",1,offset);
			display_write_str(display, "\xff \xff",2,offset);
			display_write_str(display, "\xff\xff\xff",3,offset);
			break;
		case 1:
			display_write_str(display, " \xff\xff",0,offset);
			display_write_str(display, "  \xff",1,offset);
			display_write_str(display, "  \xff",2,offset);
			display_write_str(display, "  \xff",3,offset);
			break;
		case 2:
			display_write_str(display, "\xff\xff\xff",0,offset);
			display_write_str(display, "\x07\x07\xff",1,offset);
			display_write_str(display, "\xff\x01\x01",2,offset);
			display_write_str(display, "\xff\xff\xff",3,offset);
			break;
		case 3:
			display_write_str(display, "\xff\xff\xff",0,offset);
			display_write_str(display, "\x07\x07\xff",1,offset);
			display_write_str(display, "\x01\x01\xff",2,offset);
			display_write_str(display, "\xff\xff\xff",3,offset);
			break;
		case 4:
			display_write_str(display, "\x03  ",0,offset);
			display_write_str(display, "\x03\x02 ",1,offset);
			display_write_str(display, "\x04\x05\x04",2,offset);
			display_write_str(display, " \x02 ",3,offset);
			break;
		case 5:
			display_write_str(display, "\xff\xff\xff",0,offset);
			display_write_str(display, "\xff\x07\x07",1,offset);
			display_write_str(display, "\x01\x01\xff",2,offset);
			display_write_str(display, "\xff\xff\xff",3,offset);
			break;
		case 6:
			display_write_str(display, "\xff\xff\xff",0,offset);
			display_write_str(display, "\xff\x07\x07",1,offset);
			display_write_str(display, "\xff\x01\xff",2,offset);
			display_write_str(display, "\xff\xff\xff",3,offset);
			break;
		case 7:
			display_write_str(display, "\xff\xff\xff",0,offset);
			display_write_str(display, "  \xff",1,offset);
			display_write_str(display, " \xff ",2,offset);
			display_write_str(display, " \xff ",3,offset);
			break;
		case 8:
			display_write_str(display, "\xff\xff\xff",0,offset);
			display_write_str(display, "\xff\x07\xff",1,offset);
			display_write_str(display, "\xff\x01\xff",2,offset);
			display_write_str(display, "\xff\x07\xff",3,offset);
			break;
		
		case 9:
			display_write_str(display, "\xff\xff\xff",0,offset);
			display_write_str(display, "\xff\x07\xff",1,offset);
			display_write_str(display, "\x01\x01\xff",2,offset);
			display_write_str(display, "  \xff",3,offset);
			break;
		// 10 is completly empty
		case 10:
			display_write_str(display, "   ",0,offset);
			display_write_str(display, "   ",1,offset);
			display_write_str(display, "   ",2,offset);
			display_write_str(display, "   ",3,offset);
			break;
		
		//we misuse the 11 as V
		case 11:
			display_write_str(display, "\xff \xff",0,offset);
			display_write_str(display, "\xff \xff",1,offset);
			display_write_str(display, "\x02\x07\x03",2,offset);
			display_write_str(display, " \xff ",3,offset);
			break;
		//we misuse the 12 as U
		case 12:
			display_write_str(display, "\xff \xff",0,offset);
			display_write_str(display, "\xff \xff",1,offset);
			display_write_str(display, "\xff \xff",2,offset);
			display_write_str(display, "\xff\xff\xff",3,offset);
			break;
			//we misuse the 13 as C
		case 13:
			display_write_str(display, "\xff\xff\xff",0,offset);
			display_write_str(display, "\xff  ",1,offset);
			display_write_str(display, "\xff  ",2,offset);
			display_write_str(display, "\xff\xff\xff",3,offset);
			break;
			
		//if no number was transmitted
		default:
			display_write_str(display, "\xff\xff\xff",0,offset);
			display_write_str(display, "\xff\xff\xff",1,offset);
			display_write_str(display, "\xff\xff\xff",2,offset);
			display_write_str(display, "\xff\xff\xff",3,offset);
			break;
	}
}

void display_small_number(struct DISPLAY_PAGE *display, uint8_t offset, uint8_t row, uint8_t number){

	switch (number){
		case 0:
			display_write_str(display, "0",row,offset);
		break;
		
		case 1:
			display_write_str(display, "1",row,offset);
		break;
		
		case 2:
			display_write_str(display, "2",row,offset);
		break;
		
		case 3:
			display_write_str(display, "3",row,offset);
		break;
		
		case 4:
			display_write_str(display, "4",row,offset);
		break;
		
		case 5:
			display_write_str(display, "5",row,offset);
		break;
		
		case 6:
			display_write_str(display, "6",row,offset);
		break;
		
		case 7:
			display_write_str(display, "7",row,offset);
		break;
		
		case 8:
			display_write_str(display, "8",row,offset);
		break;
		
		case 9:
			display_write_str(display, "9",row,offset);
		break;
		//if no number was transmitted
		default:
		display_write_str(display, "\xFF",row,offset);
		break;
	}
}
