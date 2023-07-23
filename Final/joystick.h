/**
 * @file joystick.h
 * @brief This file contains the definitions and function prototypes for the Joystick.
 */
#ifndef JOYSTICK_H_
#define JOYSTICK_H_

// Function prototypes
int joystick_get_x(void);
int joystick_get_y(void);
void joystick_read(void);
int joystick_get_offset_x(void);
int joystick_get_offset_y(void);



#endif /* JOYSTICK_H_ */
