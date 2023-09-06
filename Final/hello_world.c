/**
 * @file hello_world.c
 * @author  Zachary Kohlman
 * @email	 kohlmanz@msoe.edu
 * @brief This file contains the main method for the game. The game is played by
 * 			the user moving the box around the screen to avoid the enemies. The
 * 			user wins by reaching the bottom of the screen. The user loses by
 * 			touching an enemy.
 * @details
 * @version 1.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

#include "altera_avalon_pio_regs.h" 					// Altera Avalon Parallel I/O (PIO) core register map
#include "system.h" 									// System level definitions
#include "adc.h" 										// Analog to Digital Converter (ADC) functions
#include "altera_up_avalon_video_pixel_buffer_dma.h" 	// Altera Avalon Video Pixel Buffer DMA core
#include "altera_avalon_spi.h" 							// Altera Avalon Serial Peripheral Interface (SPI) core
#include "lcd.h" 										// LCD display functions
#include "altera_avalon_i2c.h"						 	// Altera Avalon Inter-Integrated Circuit (I2C) core
#include "joystick.h" 									// Joystick functions
#include "game.h" 										// Game logic functions
#include "hex_display.h" 								// Hexadecimal display functions

int main() {
	// Initialize ADC, LCD and game
	adc_init();
	lcd_init();
	game_init();

	while(1) {
		static int playing = 0; 	// Game status flag
		static int win = -1; 		// Win status flag

		// Generate first level
		generate_next_level();

		while(!playing) {
			// Draw box and follow user
			draw_box_and_follow();

			// Move enemies towards player
			move_enemies_towards_player(get_player_x(), get_player_y());

			// Check if player is touching an enemy
			if(player_touching_enemy(get_player_x(), get_player_y()) == -1) {
				playing = -1; // End game if player touches an enemy
			}

			// Check if player has reached end of level
			if(end_of_level(get_player_x(), get_player_y())) {
				if(get_level() >= 5) {
					win = 0; // Player wins if they reach level 5
					playing = -1;
					break;
				}
				reset_player_cords(); // Reset player coordinates

				// Add 3 second delay between levels
				countdown(3);

				// Generate next level
				generate_next_level();
			}
		}

		// Display game status
		if(!win) {
			hex_message("winner"); // Display "winner" if player won
		} else {
			hex_message("loser"); // Display "loser" if player lost
		}

		// Add small delay before starting again
		usleep(1000000);

		// Reset game status
		reset_game();

		// Set status to playing
		playing = 0;

		// Wait 5 seconds before starting again
		countdown(5);
	}

	return 0;
}
