/**
 ******************************************************************************
 * @file    game.h
 * @author  Zachary Kohlman
 * @email	 kohlmanz@msoe.edu
 * @version V1.0
 * @brief   Header file for game.c that contains the struct for level info
 *             and the function prototypes for game.c
 ******************************************************************************
 */

#ifndef GAME_H_
#define GAME_H_


#define NUM_LEVELS 5						// Number of levels
#define MAX_LENGTH 6						// String length to print level
#define MAX_ENEMIES 30						// Max enemies to spawn
#define ENEMY_SPAWN_RATE 5					// Number of enemies increased per level.
#define BOX_SIZE 10                         // Size of the box
#define BOX_OFFSET 1                        // Offset of the box


typedef struct {
    char level_hex[NUM_LEVELS][MAX_LENGTH];  // Array of character arrays to be displayed by HEX
    uint16_t enemy_color[NUM_LEVELS];        // Array of color codes to be iterated through for each level
    int num_enemies[NUM_LEVELS];			 // Array that defines number of enemies in current level
    int end_location[NUM_LEVELS];            // Array of horizontal line positions to be iterated through for each level
} LEVEL_INFO;

// Functions used in game.c
int generate_first_level(void);
int tetris_init(void);
int* get_enemy_x(void);
int* get_enemy_y(void);
int get_level(void);
int end_of_level(int x, int y);
void draw_en(void);
void reset_game(void);
void countdown(int num);
int get_end_dist(void);
void reset_enemy_cords(void);
#endif /* GAME_H_ */
