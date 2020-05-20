/*@file game.h
 *@author Nick Liccini
 */

#ifndef GAME_H
#define GAME_H

#include "myLib.h"
#include "objects.h"

/* #defines */
#define MAX_LIVES 3
#define MAX_BULLETS (int) (sizeof(weapon.bullets) / sizeof(weapon.bullets[0]))

#define MAX_ENEMIES 5
#define N_ENEMIES_LEVEL_1 2
#define N_ENEMIES_LEVEL_2 4
#define N_ENEMIES_LEVEL_3 5

#define MAX_OBSTACLES 5
#define N_OBSTACLES_LEVEL_1 2
#define N_OBSTACLES_LEVEL_2 4
#define N_OBSTACLES_LEVEL_3 5

#define OBSTACLE_ROW_CONSTRAINT GBA_SCREEN_HEIGHT - CRATE_HEIGHT - 10
#define OBSTACLE_COL_CONSTRAINT GBA_SCREEN_WIDTH - CRATE_WIDTH
#define WEAPON_ROW_CONSTRAINT GBA_SCREEN_HEIGHT - BLASTER_HEIGHT - 10
#define WEAPON_COL_CONSTRAINT GBA_SCREEN_WIDTH - BLASTER_WIDTH
#define ENEMY_ROW_CONSTRAINT GBA_SCREEN_HEIGHT - ZOMBIE_HEIGHT - 10
#define ENEMY_COL_CONSTRAINT GBA_SCREEN_WIDTH - ZOMBIE_WIDTH

/* External variables */
extern int paused_level;
extern int score;
extern bool readyToUpdateStatusBar;
extern u16 backgroundColor;

extern Player player1;

extern Weapon weapon;

extern Enemy enemies[];
extern int nEnemiesInLevel;
extern int speeds[];
extern int nspeeds;

extern Obstacle obstacles[];
extern int nObstacblesInLevel;

/* Button controls */
extern bool a_down;
extern bool b_down;
extern bool l_down;
extern bool r_down;

/* Prototypes */
// Initialization 
void initializeGameVariables();

// Drawing
void drawStartScreen();
void initializeNextLevel();
void drawLevelScene();
void drawGameOverScreen();
void drawGameOverWinScreen();
void drawEnemies();
void drawObstacles();
void drawWeapon();
void drawBullets();
void updateScreen();

// Object Control
void movePlayer();
void dieFromEnemy();
void grabWeapon();
void shoot();
void moveBullet();
void moveEnemies();

// Object Conditions
bool collides(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2);
bool checkHitBounds(void* o);
bool checkObstacleCollision(void* o);
bool checkEnemyOnEnemyCollision(int index, void* o);
bool checkBulletHitEnemy(Bullet* bp);
bool checkBulletHitObstacle(Bullet* bp);

// Game Execution
void execute();
void pause();
void resume();
void reset();

// Game State Conditions
bool checkKills();
bool checkGameOver();

#endif