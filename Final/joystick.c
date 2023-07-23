/**
 ******************************************************************************
 * @file    joystick.c
 * @author  Zachary Kohlman
 * @email	 kohlmanz@msoe.edu
 * @version V1.0
 * @brief   Contains the API for the joystick.

 ******************************************************************************
 */
#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "altera_modular_adc.h"
#include "joystick.h"
#include "adc.h"

// Variables to hold the joystick's x and y positions
static int joystick_x;
static int joystick_y;

// Variables to hold the joystick's x and y offsets
static int offset_x = 0;
static int offset_y = 0;

/**
 * Reads the joystick's x and y positions from the ADC channels.
 * It also calculates the offset (position from center) and calibrates the joystick.
 */
void joystick_read(void){
	int* adc_data = adc_read();

	//Calculate the offset (position from center)
	offset_x = adc_data[0] - CENTER_X;
	offset_y = adc_data[1] - CENTER_Y;

	//Calibrate the joystick
	joystick_x = adc_data[0];
	joystick_y = adc_data[1];
}

/**
 * Returns the ADC reading from channel one (joystick's x position).
 */
inline int joystick_get_x(void){
	return joystick_x;
}

/**
 * Returns the ADC reading from channel two (joystick's y position).
 */
inline int joystick_get_y(void){
	return joystick_y;
}

/**
 * Returns the offset in the x direction (position from center).
 */
inline int joystick_get_offset_x(void){
	return offset_x;
}

/**
 * Returns the offset in the y direction (position from center).
 */
inline int joystick_get_offset_y(void){
	return offset_y;
}
