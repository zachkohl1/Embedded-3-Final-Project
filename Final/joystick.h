/*
 * joystick.h
 *
 *  Created on: Apr 14, 2023
 *      Author: kohlmanz
 */

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

int joystick_get_x(void);
int joystick_get_y(void);
void joystick_read(void);
int joystick_get_offset_x(void);
int joystick_get_offset_y(void);



#endif /* JOYSTICK_H_ */
