/*
 * joystick.c
 *
 *  Created on: Apr 14, 2023
 *      Author: kohlmanz
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>

#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "alt_types.h"
#include "altera_modular_adc.h"
#include "joystick.h"
#include "adc.h"

#define center_x 119
#define center_y 120
#define x_min 0
#define x_max 242
#define y_min 0
#define y_max 242


int joystick_x;
int joystick_y;
int* adc_data;

int offset_x = 0;
int offset_y = 0;


//Sets the joy stick x and y position from ADC channels
void joystick_read(void){
	adc_data = adc_read();

	//Calculate the offset (position from center)
	offset_x = adc_data[0] - center_x;
	offset_y = adc_data[1] - center_y;

	//Calibrate the joystick
	joystick_x = adc_data[0];
	joystick_y = adc_data[1];
}

//Returns the ADC readings from channel one
int joystick_get_x(void){
	return joystick_x;
}

//Returns the ADC readings from channel 2
int joystick_get_y(void){
	return joystick_y;
}

int joystick_get_offset_x(void){
	return offset_x;
}

int joystick_get_offset_y(void){
	return offset_y;
}



