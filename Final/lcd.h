/*
 * lcd.h
 *
 *  Created on: Apr 28, 2023
 *      Author: kohlmanz
 */

#ifndef LCD_H_
#define LCD_H_

#define FRONT 0
#define BACK 1
#define GREEN 0b0000011111100000
#define TEAL 0b0000001111111111
#define RED 0b1111100000000000
#define margin_of_error_low -5
#define margin_of_error_high 5
#define margin_of_error_touch 20
#define notouch_x 340
#define max_x 3200
#define max_y 3350
#define width 320
#define height 240



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
