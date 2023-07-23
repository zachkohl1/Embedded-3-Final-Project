/*
 * joystick.c
 *
 *  Created on: Apr 14, 2023
 *      Author: kohlmanz
 */

#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "altera_modular_adc.h"
#include "joystick.h"
#include "adc.h"


static int joystick_x;
static int joystick_y;
static int offset_x = 0;
static int offset_y = 0;

//Sets the joy stick x and y position from ADC channels
void joystick_read(void){
	int* adc_data = adc_read();

	//Calculate the offset (position from center)
	offset_x = adc_data[0] - CENTER_X;
	offset_y = adc_data[1] - CENTER_Y;

	//Calibrate the joystick
	joystick_x = adc_data[0];
	joystick_y = adc_data[1];
}

//Returns the ADC readings from channel one
inline int joystick_get_x(void){
	return joystick_x;
}

//Returns the ADC readings from channel 2
inline int joystick_get_y(void){
	return joystick_y;
}

inline int joystick_get_offset_x(void){
	return offset_x;
}

inline int joystick_get_offset_y(void){
	return offset_y;
}
