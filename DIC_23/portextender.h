/*
 * portextender.h
 *
 * Created: 23.12.2022 14:09:32
 *  Author: Ole Hannemann
 */ 


#ifndef PORTEXTENDER_H_
#define PORTEXTENDER_H_


#define LED_COUNT_TOP_LEFT 5
#define LED_COUNT_TOP_RIGHT 10
#define LED_Count 15

//defines to enable disable the CS Pins
#define DIS_CS_LEFT PORTA |= (1<<PA3);
#define EN_CS_LEFT PORTA &= ~(1<<PA3);
#define DIS_CS_RIGHT PORTB |= (1<<PB7);
#define EN_CS_RIGHT PORTB &= ~(1<<PB7);

#define WASTE_CPU_TIME for(uint8_t i = 255; i > 0; ){i--;}

//ENUM für den PortExtender
enum portextender {RGB_LEFT, RGB_RIGHT};

enum led_color{PE_RED, PE_BLUE, PE_GREEN, PE_WHITE, PE_PURPLE, PE_LIGHTBLUE, PE_AMBER, PE_OFF};

struct SPI_MSG {
	enum portextender selected_extender;
	uint8_t control_byte;
	uint8_t register_addr;
	uint8_t data;
};

//configures the Required hardware for the SPI Communication to work
void configure_spi_bus();

//checks if the SPI Bus is done Sending
// 0 = SPI Bus is busy
// 1 = SPI Bus is Free
uint8_t spi_free();

//send a message over SPI
//NOTE: this probably overwrites parts of the old message beign send (if there is one atm)
void send_data(enum portextender extender, uint8_t control, uint8_t reg, uint8_t data);

//sends a message over SPI but waits until the previous message finished sending
void send_data_blocking(enum portextender extender, uint8_t control, uint8_t reg, uint8_t data);

//configures the port Extenders
void configure_portextenders();

void extender_leds(enum portextender extender, uint8_t data);
void extender_leds_blocking(enum portextender extender, uint8_t data);

void pre_defined_led_colors(enum led_color color);

void clear_top_left_bar(void);
void clear_top_right_bar(void);

void led_top_left_bar(uint16_t max_value_l, uint16_t min_value_l, uint16_t current_value_l);
void led_top_right_bar(uint16_t max_value_r, uint16_t min_value_r, uint16_t current_value_r);
void led_left_top_bar_select(uint8_t select_l);
void led_right_top_bar_select(uint8_t select_r);

void bms_error(uint8_t error);

void led_top_light(uint8_t LED);
void led_top_clear(uint8_t LED);

void led_startup_animation(uint8_t frame);

//Register Definitions
//Datenblatt Seite 5
#define PE_IODIR (uint8_t) 0x00
#define PE_IPOL (uint8_t) 0x01
#define PE_GPINTEN (uint8_t) 0x02
#define PE_DEFVAL (uint8_t) 0x03
#define PE_INTCON (uint8_t) 0x04
#define PE_IOCON (uint8_t) 0x05
#define PE_GPPU (uint8_t) 0x06
#define PE_INTF (uint8_t) 0x07
//READ ONLY!
#define PE_INTCAP (uint8_t) 0x08
#define PE_GPIO (uint8_t) 0x09
#define PE_OLAT (uint8_t) 0x0a


// Schreib operarionen haben das RW bit als 0 -> Read Operation haben RW als 1 gesetzt
//
// Control Byte Aufbau
//	------------------------------------
//  | 0 | 1 | 0 | 0 | 0 | A1 | A0 | RW |
//  ------------------------------------
//
// A1,A0 sind Adress Bits falls man mehrere Geräte auf dem selben SPI Bus hat (hardware Pins)
//
// IODIR
// -> INPUT = 1; OUTPUT = 0
// BY Default alles INPUT
//
// IOCON (Configuration Register)
// 
// ----------------------------------------------------
// | - | - | SEQOP | DISSLW | HAEN | ODR | INTPOL | - | 
// ----------------------------------------------------
//
// SEQOP: Sequential Operation Mode
// 1 = Sequention operation disabled
// 0 = Sequential Operation enabled
// 
// DISSLW: Slew Rate Control Bit for SDA Output
// 1 = Slew Rate disabled
// 0 = Slew Rate enabled
//
// HAEN: Hardware Adress Enable
// 1 = Enabled
// 0 = Disabled
//
// ODR: Open Drain Output des INT Pin
// 1 = Open Drain Output (überschreibt INTPOL bit)
// 0 = Active Driver Output (INTPOL bit setzt Polarität)
//
// INTPOL: Setzt die polarität des INT Output Pin
// 1 = Active High
// 0 = Active Low
//
// Notiz ins GPIO register schreiben schreibt in da OLAT Register!

// LEFT
// ---------------------------------------------------
// | x | x | RL_B | FL_B | FL_R | FL_G | RL_G | RL_R |
// ---------------------------------------------------

// RIGHT
// ---------------------------------------------------
// | x | x | RR_B | FR_B | FR_R | FR_G | RR_G | RR_R |
// ---------------------------------------------------

// PORT DEFINES

#define R_RED 0
#define R_GREEN 1
#define F_GREEN 2
#define F_RED 3
#define F_BLUE 4
#define R_BLUE 5

#endif /* PORTEXTENDER_H_ */