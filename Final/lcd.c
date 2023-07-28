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


/**
 * @brief Initializes the LCD screen.
 *
 * This function initializes the ADC, opens the pixel buffer DMA device,
 * allocates memory for the back buffer, swaps the buffers, and clears the front buffer.
 * It also sets the initial position of the box to the top left corner of the screen (0,0).
 */
void lcd_init(void) {
	adc_init();
	usleep(100);
	// open the device, note the "Linux" driver format
	lt24 = alt_up_pixel_buffer_dma_open_dev("/dev/video_pixel_buffer_dma_0");

	alt_up_pixel_buffer_dma_change_back_buffer_address(lt24,(unsigned int)malloc(WIDTH*HEIGHT*2));
	alt_up_pixel_buffer_dma_swap_buffers(lt24);

	while(alt_up_pixel_buffer_dma_check_swap_buffers_status(lt24))
	{ /* make sure swap is done */ }
	alt_up_pixel_buffer_dma_change_back_buffer_address(lt24,(unsigned int)malloc(WIDTH*HEIGHT*2));

	// clear front buffers
	alt_up_pixel_buffer_dma_clear_screen(lt24,FRONT);

	//Set initial box position to to left (0,0)
	x = 0;
	y = 0;
}

/**
 * @brief Draws a box and updates its position based on joystick input.
 *
 * This function clears the back buffer of the screen, generates enemies,
 * draws a goal line at a specified location, and draws a green 10x10 box
 * at the current player position. It then updates the player position based
 * on joystick input and swaps the front and back buffers.
 */
void draw_box_and_follow(void){

	// Clear screen
	alt_up_pixel_buffer_dma_clear_screen(lt24, BACK);

	//Genereate enemies
	draw_en();

	//Draw horizontal line	//Draw the goal line at location
	alt_up_pixel_buffer_dma_draw_hline(lt24,0,WIDTH, get_end_dist(), RED, BACK);

	// Draw a green 10x10 box at the updated player position
	alt_up_pixel_buffer_dma_draw_box(lt24, 0 + x, 0 + y, BOX_SIZE + x, BOX_SIZE + y, GREEN, BACK);

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
	if(joystick_get_offset_y() < MARGIN_OF_ERROR_LOW){
		//Y-Center < -5
		//Down
		y-=5;
	} else if(joystick_get_offset_y() > MARGIN_OF_ERROR_HIGH){
		//Y-Center > 5
		//Up
		y+=5;
	}

	if(joystick_get_offset_x() < MARGIN_OF_ERROR_LOW){
		//X-Center < -5
		//right
		x+=5;
	} else if(joystick_get_offset_x() > MARGIN_OF_ERROR_HIGH){
		//X-Center > 5
		//left
		x-=5;
	}

	//Check boundaries
	if(x < 0) x = 0;
	if(y  < 0) y = 0;
	if(x > WIDTH-BOX_SIZE) x = WIDTH-BOX_SIZE;
	if(y > HEIGHT-BOX_SIZE) y = HEIGHT-BOX_SIZE;
}


// Mode
const uint8_t getx = 0x92;
const uint8_t gety =  0xD2;

uint8_t recieve[2];

/**
 * @brief This function reads the touch coordinates from the SPI interface.
 *
 * The function sends SPI commands to read the x and y coordinates of the touch.
 * The received coordinates are then adjusted to match the screen's width and height.
 * The y coordinate is also inverted to match the screen's coordinate system.
 */
void follow_touch(void){
	alt_avalon_spi_command(SPI_0_BASE,0,1,&getx,2,recieve,0);
	xtouch = (recieve[0] << 5 | recieve[1] >> 3);
	alt_avalon_spi_command(SPI_0_BASE,0,1,&gety,2,recieve,0);
	ytouch = recieve[0] << 5 | recieve[1]>>3;

	//Adjust to be in terms of height and width and not pixels
	xtouch = (xtouch * WIDTH) / MAX_X;
	ytouch = abs(((ytouch * HEIGHT) / MAX_Y)-HEIGHT);
}


/**
 * @brief Checks if the touch screen is being touched.
 *
 * This function first calls the follow_touch() function to update the xtouch variable.
 * It then checks if the xtouch value is less than the notouch_x value.
 * The notouch_x value represents the x-coordinate when the screen is not being touched.
 * If the xtouch value is less than notouch_x, it means the screen is being touched and the function returns 1.
 * Otherwise, it returns 0 indicating the screen is not being touched.
 *
 * @return int Returns 1 if the screen is being touched, otherwise returns 0.
 */
int isTouching(void){
	//Set x
	follow_touch();
	//Return 1 if larger than max
	return(xtouch < NO_TOUCH_X);
}



/**
 * @brief Draws a box on the screen at the touch location.
 *
 * This function checks if the screen is being touched by calling the isTouching() function.
 * If the screen is being touched, it waits until the pixel buffer is ready to be written to.
 * It then cycles through an array of colors, drawing a box at the touch location with the current color.
 * After drawing the box, it swaps the pixel buffers to display the new box.
 * If the screen is not being touched, it checks if the screen has been drawn on before.
 * If it has, it simply swaps the pixel buffers. If it hasn't, it clears the screen and then swaps the pixel buffers.
 */
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
		usleep(SHORT_DELAY);
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
 * @brief Draws a box on the screen at the specified location.
 *
 * This function draws a box on the screen at the specified x and y coordinates.
 * The box is drawn with the specified color.
 *
 * @param x The x-coordinate of the top-left corner of the box.
 * @param y The y-coordinate of the top-left corner of the box.
 * @param color The color of the box.
 */
void draw_box(int x, int y, uint16_t color){
	alt_up_pixel_buffer_dma_draw_box(lt24, 0+x, 0+y, 10+x, 10+y, color, BACK);
}

/**
 * @brief Clears the screen.
 *
 * This function clears the screen by setting all pixels to the background color.
 */
void clear_screen(void){
	alt_up_pixel_buffer_dma_clear_screen(lt24,FRONT);
}

/**
 * @brief Gets the x-coordinate of the player.
 *
 * This function returns the current x-coordinate of the player.
 *
 * @return int The current x-coordinate of the player.
 */
int get_player_x(void){
	return x;
}

/**
 * @brief Gets the y-coordinate of the player.
 *
 * This function returns the current y-coordinate of the player.
 *
 * @return int The current y-coordinate of the player.
 */
int get_player_y(void){
	return y;
}

/**
 * @brief Resets the player's coordinates.
 *
 * This function resets the player's x and y coordinates to 0.
 */
void reset_player_cords(void){
	//Sets the cords to 0,0
	x = 0;
	y = 0;
}







