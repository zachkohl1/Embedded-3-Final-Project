/**
 * @file hex_display.h
 * @brief This file is to be used to declare the functions and data types used in hex_display.c
 */

#ifndef HEX_DISPLAY_H_
#define HEX_DISPLAY_H_

// Hex display definitions
#define CENTER_X  119
#define CENTER_Y 120
#define MAX_LENGTH 6
#define HEX5_4_SHIFT_MAX 8
#define HEX3_0_SHIFT_MAX 24
#define WORD 8

// Function prototypes
void hex_message(const char* string);

#endif /* HEX_DISPLAY_H_ */
