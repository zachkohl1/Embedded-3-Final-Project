/**
 * This is the my final lab to build a tetris game that uses
 * the accelerameter.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "adc.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_avalon_spi.h"
#include "lcd.h"
#include "altera_avalon_i2c.h"
#include "joystick.h"
#include "game.h"
#include "hex_display.h"

int main()
{
	//Inits
	adc_init();
	lcd_init();
	tetris_init();

	while(1){
		int playing = 0;
		int win = -1;

		//Generate first level
		generate_next_level();

		while(!playing){
			//Follow user
			draw_box_and_follow();

			//Determine if current position hits an enemy or is at last level
			if(player_touching_enemy(get_player_x(), get_player_y()) == -1){
				playing=-1;
			}

			if(end_of_level(get_player_x(), get_player_y())){
				if(get_level() >= 5){
					win = 0;
					playing=-1;
					break;
				}
				reset_player_cords();

				//add 3 second delay between levels
				countdown(3);

				//Next level
				generate_next_level();
			}
		}

		//Display status
		if(!win){
			hex_message("winner");
		} else {
			hex_message("loser");
		}

		//Add small delay before starting again
		usleep(1000000);

		//Reset game status
		reset_game();

		//Wait 5 seconds
		countdown(5);


	}

	return 0;
}
