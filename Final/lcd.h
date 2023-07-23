/**
 * @file lcd.h
 * @brief This file contains the definitions and function prototypes for the LCD display.
 */

#ifndef LCD_H_
#define LCD_H_

// Definitions for the LCD display
#define FRONT 0
#define BACK 1
#define GREEN 0b0000011111100000
#define RED 0b1111100000000000
#define MARGIN_OF_ERROR_LOW -5
#define MARGIN_OF_ERROR_HIGH 5
#define MARGIN_OF_ERROR_TOUCH 20
#define NO_TOUCH_X 340
#define MAX_X 3200
#define MAX_Y 3350
#define WIDTH 320
#define HEIGHT 240
#define SHORT_DELAY 6200

// Function prototypes
void lcd_init(void);
void draw_box_and_follow(void);
void follow_joystick(void);
void follow_touch(void);
void touch_draw(void);
void draw_horizontal_line(int x0, int x1, int y, int color);
void draw_box(int x, int y, uint16_t color);
void clear_screen(void);
int get_player_x(void);
int get_player_y(void);
void reset_player_cords(void);

#endif /* LCD_H_ */
