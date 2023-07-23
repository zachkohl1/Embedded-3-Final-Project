/**
 ******************************************************************************
 * @file hello_world.c
 * @author Zachary Kohlman
 * @email kohlmanz@msoe.edu
 * @version V1.0
 * @brief This file contains the functionality assoicated with moving a
 * green box on an LCD screen with a joy stick controller using the
 * avalon provided interface.
 ******************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stddef.h>

#include "altera_avalon_pio_regs.h"
#include "system.h"
#include "adc.h"
#include "joystick.h"
#include "altera_up_avalon_video_pixel_buffer_dma.h"
#include "altera_avalon_spi.h"
#include "lcd.h"
#include "hex_display.h"
#include "game.h"

//Constants and Variables used through methods
alt_up_pixel_buffer_dma_dev* lt24;
static int x = 0;
static int y = 0;
static short xtouch = 0;
static short ytouch = 0;
int screen_drawn = 0;
// List of colors
unsigned short colors[] = {
		0x001F,  // Blue
		0xF800,  // Red
		0x07E0,  // Green
		0xFFE0,  // Yellow
		0xF81F,  // Magenta
		0x07FF,  // Cyan
		0xFFFF   // White
};
int num_colors = sizeof(colors)/sizeof(colors[0]);

void lcd_init(void) {
	adc_init();
	usleep(100);
	// open the device, note the "Linux" driver format
	lt24 = alt_up_pixel_buffer_dma_open_dev("/dev/video_pixel_buffer_dma_0");

	alt_up_pixel_buffer_dma_change_back_buffer_address(lt24,(unsigned int)malloc(320*240*2));
	alt_up_pixel_buffer_dma_swap_buffers(lt24);

	while(alt_up_pixel_buffer_dma_check_swap_buffers_status(lt24))
	{ /* make sure swap is done */ }
	alt_up_pixel_buffer_dma_change_back_buffer_address(lt24,(unsigned int)malloc(320*240*2));

	// clear front buffers
	alt_up_pixel_buffer_dma_clear_screen(lt24,FRONT);

	//Set initial box position to to left (0,0)
	x = 0;
	y = 0;
}

void draw_box_and_follow(void){

    // Clear screen
    alt_up_pixel_buffer_dma_clear_screen(lt24, BACK);

    //Genereate enemies
    draw_en();

    //Draw horizontal line	//Draw the goal line at location
	alt_up_pixel_buffer_dma_draw_hline(lt24,0,width, get_end_dist(), RED, BACK);

    // Draw a green 10x10 box at the updated player position
    alt_up_pixel_buffer_dma_draw_box(lt24, 0 + x, 0 + y, 10 + x, 10 + y, GREEN, BACK);

    // Update player position based on joystick input
    follow_joystick();

    // Swap buffers
    alt_up_pixel_buffer_dma_swap_buffers(lt24);
}


void follow_joystick(void){
	joystick_read();

	/**
	 * Measured offset values:
	 * Center (0,0)
	 * Up: (0, -119)
	 * Down: (-1, 123)
	 * Left: (123, -1)
	 * Right: (-119, 0)
	 *
	 */

	//Check if moved down by checking against its variability (M.O.E)
	if(joystick_get_offset_y() < margin_of_error_low){
		//Y-Center < -5
		//Down
		y-=5;
	} else if(joystick_get_offset_y() > margin_of_error_high){
		//Y-Center > 5
		//Up
		y+=5;
	}

	if(joystick_get_offset_x() < margin_of_error_low){
		//X-Center < -5
		//right
		x+=5;
	} else if(joystick_get_offset_x() > margin_of_error_high){
		//X-Center > 5
		//left
		x-=5;
	}

	//Check boundaries
	if(x < 0) x = 0;
	if(y  < 0) y = 0;
	if(x > width-10) x = width-10;
	if(y > height-10) y = height-10;
}

void follow_touch(void){
	//Return code. Not used
	int return_code = 0;

	//Mode
	const uint8_t getx = 0x92;
	const uint8_t gety =  0xD2;

	uint8_t recieve[2];

	return_code = alt_avalon_spi_command(SPI_0_BASE,0,1,&getx,2,recieve,0);
	xtouch = (recieve[0] << 5 | recieve[1] >> 3);
	return_code = alt_avalon_spi_command(SPI_0_BASE,0,1,&gety,2,recieve,0);
	ytouch = recieve[0] << 5 | recieve[1]>>3;
	printf("X: %10d, Y: %10d\n", xtouch,ytouch);

	//Adjust to be in terms of height and width and not pixels
	xtouch = (xtouch * width) / max_x;
	ytouch = abs(((ytouch * height) / max_y)-height);


}


int isTouching(void){
	//Set x
	follow_touch();
	//Return 1 if larger than max
	return(xtouch < notouch_x);
}



void draw_touch(void) {
	if (isTouching()) {
		while(alt_up_pixel_buffer_dma_check_swap_buffers_status(lt24)) {
			/* make sure swap is done */
		}

		// Cycle through the colors in the array
		static int color_index = 0;
		unsigned short color = colors[color_index];
		color_index = (color_index + 1) % num_colors;

		// Draw a box with the current color
		alt_up_pixel_buffer_dma_draw_box(lt24, xtouch, ytouch, xtouch+7, ytouch+7, color, BACK);

		// Swap buffers
		alt_up_pixel_buffer_dma_swap_buffers(lt24);

		// Set the screen_drawn variable to 1, indicating that the screen has been drawn on
		screen_drawn = 1;
		//Add short delay
		usleep(6200);
	}
	else {
		if (screen_drawn) {
			// No longer touching, but the screen has been drawn on before, so don't clear
			alt_up_pixel_buffer_dma_swap_buffers(lt24);
		} else {
			// No longer touching and the screen has not been drawn on yet, so clear
			alt_up_pixel_buffer_dma_clear_screen(lt24,BACK);
			alt_up_pixel_buffer_dma_swap_buffers(lt24);
		}
	}
}


/**
 * Draws enemy boxes at location x and y
 */
void draw_box(int x, int y, uint16_t color){
	alt_up_pixel_buffer_dma_draw_box(lt24, 0+x, 0+y, 10+x, 10+y, color, BACK);

}

void clear_screen(void){
	alt_up_pixel_buffer_dma_clear_screen(lt24,FRONT);

}

int get_player_x(void){
	return x;
}

int get_player_y(void){
	return y;
}

void reset_player_cords(void){
	//Sets the cords to 0,0
	x = 0;
	y = 0;
}







