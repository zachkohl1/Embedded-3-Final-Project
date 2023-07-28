/**
 ******************************************************************************
 * @file    hex_display.c
 * @author  Zachary Kohlman
 * @email	 kohlmanz@msoe.edu
 * @version V1.0
 * @brief   This file constains the hex_message API used in the main method.
 * 			The function simply prints a string in left to right order on the
 * 			HEX displays.

 ******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "hex_display.h"

//Pointers to either side of the HEX display
volatile uint32_t* const hex5_4 = (uint32_t*)HEX5_HEX4_BASE;
volatile uint32_t* const hex3_0 = (uint32_t*)HEX3_HEX0_BASE;

/**
 * Name: hex_message
 * Provides: A count down and assorted messages to complete milestone 2.
 * I use strncpy to copy the ascii I want to display to the hexs to the
 * hex address pointer.
 * Arguments: None
 * Returns: None
 */

void hex_message(const char* string){

	//Temporary variable to help iterate through the second set of HEXs
	int temp = 0;

	//Clear all digits
	*hex5_4 = 0;
	*hex3_0 = 0;

	//If string is greater than 6 characters, print ERR
	if(strlen(string)> 6){
		*hex3_0 |= ('e' << (WORD*2)) | ('r' << WORD) | ('r');
	}else{
		//Not less than iterate through each of the characters
		//and print them on the HEX's left to right
		for(int i = 0; i < strlen(string); i++){
			if(i < 2){
				*hex5_4 |= string[i] << (HEX5_4_SHIFT_MAX - (HEX5_4_SHIFT_MAX*i));
			} else {
				*hex3_0 |= string[i] << (HEX3_0_SHIFT_MAX-(temp*WORD));
				temp++;
			}
		}
	}
}






