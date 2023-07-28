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

// Game definitions
#define NUM_LEVELS 5						// Number of levels
#define MAX_LENGTH 6						// String length to print level
#define MAX_ENEMIES 20						// Max enemies to spawn
#define ENEMY_SPAWN_RATE 5					// Number of enemies increased per level.
#define BOX_SIZE 10                         // Size of the box
#define BOX_OFFSET 1                        // Offset of the box
#define MAX_ATTEMPTS 1000                   // Max attempts to generate a new enemy
#define SAFE_DISTANCE 10                    // Safe distance between player and enemy
#define END_OF_GAME_DELAY 1000000           // Delay at end of game
#define HALF_SECOND_DELAY 250000            // Delay for half a second


typedef struct {
    char level_hex[NUM_LEVELS][MAX_LENGTH];  // Array of character arrays to be displayed by HEX
    uint16_t enemy_color[NUM_LEVELS];        // Array of color codes to be iterated through for each level
    int num_enemies[NUM_LEVELS];			 // Array that defines number of enemies in current level
    int end_location[NUM_LEVELS];            // Array of horizontal line positions to be iterated through for each level
} LEVEL_INFO;

// Function prototypes
void generate_next_level(void);
int game_init(void);
int get_enemy_x(int index);
int get_enemy_y(int index);
int get_level(void);
int end_of_level(int x, int y);
void draw_en(void);
void reset_game(void);
void countdown(int num);
int get_end_dist(void);
void reset_enemy_cords(void);
void update_enemy_positon(int index, int x, int y);
int player_touching_enemy(int x, int y);
int get_num_enemies(void);
void move_enemies_towards_player(int player_x, int player_y);
#endif /* GAME_H_ */
