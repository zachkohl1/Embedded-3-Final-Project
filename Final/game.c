/**
 ******************************************************************************
 * @file    game.c
 * @author  Zachary Kohlman
 * @email	 kohlmanz@msoe.edu
 * @version V1.0
 * @brief   Contains the game logic for the game. This includes the level
 * 			generation, enemy generation, and collision detection.
 * 			Also contains the reset function to reset the game to the
 * 			beginning. The game is played by the user moving the box
 * 			around the screen to avoid the enemies. The user wins by
 * 			reaching the bottom of the screen. The user loses by
 * 			touching an enemy.
 ******************************************************************************
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>
#include <time.h>
#include <string.h>


#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "adc.h"
#include "joystick.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_avalon_spi.h"
#include "lcd.h"
#include "hex_display.h"
#include "game.h"

// Game variables
static int level_count = 0;						// Level counter
static int num_enemies = 0;						// Enemy counter
static int  end_position = 70;					// To bottom of the screen
static int end_location = 0;					// Horizontal line position

// Game constants
static const int ENEMY_SPEED =  1;              // Enemy speed


// Game structs
LEVEL_INFO level_info;							// Struct that contains all level info
char str[MAX_LENGTH];							// String to display on 7-seg displays
int xcord[MAX_ENEMIES];							// Array of x coordinates for enemies. Uses sentinel value of -1
int ycord[MAX_ENEMIES];							// Array of y coordinates for enemies. Uses sentinel value of -1

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
int game_init(void){
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
 * This function generates the next level in the game.
 * It clears the screen, increments the level counter, and displays the new level on 7-seg displays.
 * It then draws enemies at random x and y locations. These enemies are then moved towards the player at a constant speed.
 * If a valid position for an enemy cannot be found after a certain number of attempts, an error message is printed.
 *
 * @return 0 on successful execution.
 */
void generate_next_level(void){
	// Clear the screen
	clear_screen();

	// Increase Level counter
	level_count++;

	// Display level on Hex displays, decrement level_count to index at 0
	int current_level_index = level_count - 1;
	hex_message(level_info.level_hex[current_level_index]);

	// Draw enemies at random x and y locations. Are stationary.
	num_enemies = level_info.num_enemies[current_level_index];
	end_location = level_info.end_location[current_level_index];

	for(int i = 0; i < num_enemies; i++){
		int x, y;
		int attempts = 0;

		while (1) {
			// Generate random position
			x = (rand() % (WIDTH - BOX_SIZE - BOX_OFFSET)) + BOX_OFFSET;
			y = (rand() % (end_location - BOX_SIZE - BOX_OFFSET)) + BOX_OFFSET;
			// Check for collision with existing enemies and the player
			int j;
			for (j = 0; j < i; j++) {
				if ((abs(xcord[j] - x) < BOX_SIZE && abs(ycord[j] - y) < BOX_SIZE) ||
					(abs(get_player_x() - x) < (BOX_SIZE + SAFE_DISTANCE) && abs(get_player_y() - y) < (BOX_SIZE + SAFE_DISTANCE))) {
					break;  // Collision detected, break out of the loop
				}
			}

			if (j == i || ++attempts == MAX_ATTEMPTS) {
				// No collision detected or max attempts reached, break out of the while loop
				break;
			}
		}

		if (attempts == MAX_ATTEMPTS) {
			// Handle situation where a valid position couldn't be found
			printf("Error: Could not find a valid position for enemy %d after %d attempts\n", i, MAX_ATTEMPTS);
		} else {
			// Update Arrays
			xcord[i] = x;
			ycord[i] = y;
		}
	}
}
/**
 * Returns -1 if touching enemy
 */
int player_touching_enemy(int x, int y){
	int player_box_right = x + BOX_SIZE; // Rightmost x-coordinate of player's box
	int player_box_bottom = y + BOX_SIZE; // Bottommost y-coordinate of player's box

	for(int i = 0; i < num_enemies; i++){

		// Determine if the current value is not a sentinel value
		if(xcord[i] < 0 || ycord[i] < 0){
			break;                // Stop
		}

		// Check if player's box touches the current enemy block
		int enemy_box_right = xcord[i] + BOX_SIZE; // Rightmost x-coordinate of enemy's box
		int enemy_box_bottom = ycord[i] + BOX_SIZE; // Bottommost y-coordinate of enemy's box

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
	num_enemies = 0;
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
		usleep(HALF_SECOND_DELAY);
	}
}

/**
 * Returns the end distance for the current level
*/
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

/**
 * Updates the x and y coordinates of the enemy at the given index
 */
void update_enemy_position(int index, int x, int y){
	xcord[index] = x;
	ycord[index] = y;
}

/**
 * Returns the x coordinate of the enemy at the given index
 */
int get_enemy_x(int index){
	return xcord[index];
}

/**
 * Returns the y coordinate of the enemy at the given index
 */
int get_enemy_y(int index){
	return ycord[index];

}

int get_num_enemies(void){
	return num_enemies;
}

void move_enemies_towards_player(int player_x, int player_y){
	for(int i = 0; i < num_enemies; i++){
		// Determine if the current value is not a sentinel value
		if(xcord[i] < 0 || ycord[i] < 0){
			break;                // Stop
		}

		// Move enemy towards player
		int x_diff = player_x - xcord[i];
		int y_diff = player_y - ycord[i];

		// Move enemy towards player
		if(abs(x_diff) > abs(y_diff)){
			// Move enemy towards player in x direction
			if(x_diff > 0){
				// Move enemy right
				xcord[i] += ENEMY_SPEED;
			}else{
				// Move enemy left
				xcord[i] -= ENEMY_SPEED;
			}
		}else{
			// Move enemy towards player in y direction
			if(y_diff > 0){
				// Move enemy down
				ycord[i] += ENEMY_SPEED;
			}else{
				// Move enemy up
				ycord[i] -= ENEMY_SPEED;
			}
		}
	}
}
