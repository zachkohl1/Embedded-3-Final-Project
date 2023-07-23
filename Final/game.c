/**
 ******************************************************************************
 * @file    game.c
 * @author  Zachary Kohlman
 * @email	 kohlmanz@msoe.edu
 * @version V1.0
 * @brief   Contains the game logic for the game. This includes the level
 * 			generation, enemy generation, and collision detection.
 * 			Also contains the reset function to reset the game to the
 * 			beginning.
 ******************************************************************************
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>
#include <time.h>


#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "adc.h"
#include "joystick.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_avalon_spi.h"
#include "lcd.h"
#include "hex_display.h"
#include "game.h"


static int level_count = 0;						// Level counter
static int num_enemies = 5;						// Enemy counter
static int  end_position = 70;					// To bottom of the screen
LEVEL_INFO level_info;							// Struct that contains all level info
char str[MAX_LENGTH];							// String to display on 7-seg displays
int xcord[MAX_ENEMIES];							// Array of x coordinates for enemies
int ycord[MAX_ENEMIES];							// Array of y coordinates for enemies

// Array of colors to be iterated through for each level
static const uint16_t enemy_colors[] = {
		0xFFFF,   	// White
		0x001F,  	// Blue
		0xFFE0,  	// Yellow
		0xF81F,  	// Magenta
		0x07FF  	// Cyan
};

// Array of character arrays to be displayed by 7-seg displays
static const char* level_names[] = {
		"lvl 1",
		"lvl 2",
		"lvl 3",
		"lvl 4",
		"lvl 5"
};

/**
 * Populates the struct with values and sets constants
 */
int tetris_init(void){
	level_count = 0;
	srand(time(NULL));   // Initialization for rand. num

	for (int i = 0; i < NUM_LEVELS; i++) {

		strcpy(level_info.level_hex[i], level_names[i]);

		// Add enemy color. Cycles through array
		level_info.enemy_color[i] = enemy_colors[i];

		// Add number of enemies. Increase by 5
		level_info.num_enemies[i] = num_enemies+=ENEMY_SPAWN_RATE;

		// Add ending location. Increase by 20 (lower to bottom)
		level_info.end_location[i] = end_position += 20;
	}

	//Fill enemy X and Y coordinates with sentinel values
	reset_enemy_cords();

	return 0;
}

/**
 * Generates a new level. 5 is the max, and each time, the horizontal line gets closer to the top
 * and the "enemy" blocks are faster
 */
int generate_next_level(void){
	// clear_screen();
	// //Increase Level counter.
	// level_count++;

	// //Display level on Hex displays, decrement level_count to index at 0
	// hex_message(level_info.level_hex[level_count-1]);

	// //Draw enemies at random x and y locations. Are stationary
	// for(int i = 0; i < level_info.num_enemies[level_count-1]; i++){
	// 	//Only draw if not a sentinel value

	// 	//Enemies are 10 x 10 boxes with random positive coordinates on screen
	// 	int x = rand()%(width-10)+1;
	// 	int y = rand()%(level_info.end_location[level_count-1]-10)+1;

		//Update Arrays
		xcord[i] = x;
		ycord[i] = y;

	}
	return 0;
}

/**
 * Returns -1 if touching enemy
 */
int player_touching_enemy(int x, int y){
	int player_box_right = x + 10; // Rightmost x-coordinate of player's box
	int player_box_bottom = y + 10; // Bottommost y-coordinate of player's box

	for(int i = 0; i < MAX_ENEMIES; i++){
		// Determine if the current value is not a sentinel value
		if(xcord[i] < 0 || ycord[i] < 0){
			break;                // Stop
		}

		// Check if player's box touches the current enemy block
		int enemy_box_right = xcord[i] + 10; // Rightmost x-coordinate of enemy's box
		int enemy_box_bottom = ycord[i] + 10; // Bottommost y-coordinate of enemy's box

		// Check for overlap or touching
		if (player_box_right >= xcord[i] && x <= enemy_box_right &&
				player_box_bottom >= ycord[i] && y <= enemy_box_bottom) {
			return -1; // Player's box touches an enemy block
		}
	}

	return 0; // Player's box does not touch any enemy block
}

/**
 * Returns level count, starts at 1 and ends at 6
 */
int get_level(void){
	return level_count;
}

/**
 * Returns true if the box is on the h.line
 *
 */
int end_of_level(int x, int y){
	return (y >= level_info.end_location[level_count-1]);
}

/**
 * Draws the enemies. 10-30
 */
void draw_en(void){
	for(int i = 0; i < level_info.num_enemies[level_count -1]; i++){
		draw_box(xcord[i],ycord[i], level_info.enemy_color[level_count -1]);
	}
}

/**
 * Resets the state of the game to beginning
 */
void reset_game(void){
	level_count = 0;
	num_enemies = 5;
	end_position = 100;
	reset_player_cords();
	reset_enemy_cords();

}

/**
 * Counts from 1 to num seconds
 */
void countdown(int num){
	for(int i = num; i >= 1; i-- ){
		//Convert number to a string
		sprintf(str, "%d",i);
		hex_message(str);
		//Wait for 1/2 a second
		usleep(250000);
	}
}

int get_end_dist(void){
	return level_info.end_location[level_count-1];
}

/**
 * Resets the xcord and ycord arrays with -1 as sentinel values
 */
void reset_enemy_cords(void){
	//Reset enemy coordinates
	for(int i = 0; i < MAX_ENEMIES; i++){
		xcord[i] = -1;
		ycord[i] = -1;
	}
}

