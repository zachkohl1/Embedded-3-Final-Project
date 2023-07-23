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
 * @brief This function displays a message on a HEX display.
 * 
 * The function takes a string as input and displays it on a HEX display.
 * The string is displayed in left to right order. If the string is null,
 * an error message is printed. If the length of the string is more than 6,
 * 'err' is displayed on the HEX display. The string is split between two
 * parts of the HEX display, hex5_4 and hex3_0, with the first two characters
 * going to hex5_4 and the rest going to hex3_0.
 *
 * @param string The string to be displayed on the HEX display.
 */
void hex_message(const char* string){

	//Checks if the string is null
	if (string == NULL) {
		printf("Error: Input string is null\n");
		return;
	}

	int temp = 0;	//Used to keep track of the index of the string
	*hex5_4 = 0;	//Clears the hex5_4 display
	*hex3_0 = 0;	//Clears the hex3_0 display

	int length = strlen(string);	//Gets the length of the string

	//Checks if the string is too long
	if(length > MAX_LENGTH){
		*hex3_0 |= ('e' << (WORD*2)) | ('r' << WORD) | ('r');
	}else{
		//Iterates through the string and displays it on the HEX displays
		for(int i = 0; i < length; i++){
			if(i < 2){
				*hex5_4 |= string[i] << (HEX5_4_SHIFT_MAX - (HEX5_4_SHIFT_MAX*i));
			} else {
				*hex3_0 |= string[i] << (HEX3_0_SHIFT_MAX-(temp*WORD));
				temp++;
			}
		}
	}
}






