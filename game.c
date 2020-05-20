/*@file game.c
 *@author Nick Liccini
 */

#include "myLib.h"
#include "text.h"
#include "game.h"

/* Initialize External Variables */
int paused_level = 0;
int score = 0;
bool readyToUpdateStatusBar = TRUE;
u16 backgroundColor = BLACK;

Player player1 = { 0, 0, PLAYER_WIDTH, PLAYER_HEIGHT, 0, 0, MAX_LIVES, FALSE, 2, 1, 0, player };

Enemy enemies[MAX_ENEMIES];
int nEnemiesInLevel = 0;

Obstacle obstacles[MAX_OBSTACLES];
int nObstaclesInLevel = 0;

Weapon weapon;

int speeds[] = { -1, 1 };
int nspeeds = sizeof(speeds) / sizeof(speeds[0]);

bool a_down = FALSE;
bool b_down = FALSE;
bool l_down = FALSE;
bool r_down = FALSE;


/* Initialization */
/* initializeGameVariables
*	Called at the start of each new level
*	Updates the constants for the level
*	Resets the player variables
*	Creates new randomized obstacles
*	Creates new randomized enemies
*	Creates a new randomized weapon
*/
void initializeGameVariables() {
	readyToUpdateStatusBar = TRUE;
	
	// Set constants for the current level
	switch(player1.level) {
		case 1:
			nObstaclesInLevel = N_OBSTACLES_LEVEL_1;
			nEnemiesInLevel = N_ENEMIES_LEVEL_1;
			backgroundColor = GREEN;
			break;
		case 2:
			nObstaclesInLevel = N_OBSTACLES_LEVEL_2;
			nEnemiesInLevel = N_ENEMIES_LEVEL_2;
			backgroundColor = YELLOW;
			break;
		case 3:
			nObstaclesInLevel = N_OBSTACLES_LEVEL_3;
			nEnemiesInLevel = N_ENEMIES_LEVEL_3;
			backgroundColor = RED;
			break;
	}

	// Reset player members for each new level
	player1.x = 0;
	player1.y = 0;
	player1.oldPosX = 0;
	player1.oldPosY = 0;
	player1.kills = 0;
	player1.hasWeapon = FALSE;

	// Build new obstacles for each level
	for (int i = 0; i < nObstaclesInLevel; ++i) {
		Obstacle* op = &(obstacles[i]);
		op->x = rand() % OBSTACLE_COL_CONSTRAINT;
		op->y = rand() % OBSTACLE_ROW_CONSTRAINT;
		op->width = CRATE_WIDTH;
		op->height = CRATE_HEIGHT;
		op->image = crate;

		// Make sure obstacles don't appear in the player spawn or out of bounds
		while (checkHitBounds(op) || collides(op->x, op->y, op->width, op->height, 0, 0, player1.width + 20, player1.height + 20)) {
			op->x = rand() % OBSTACLE_COL_CONSTRAINT;
			op->y = rand() % OBSTACLE_ROW_CONSTRAINT;
		}
	}

	// Build new enemies for each level
	for (int i = 0; i < nEnemiesInLevel; ++i) {
		Enemy* ep = &(enemies[i]);
		ep->x = rand() % ENEMY_COL_CONSTRAINT;
		ep->y = rand() % ENEMY_ROW_CONSTRAINT;
		ep->width = ZOMBIE_WIDTH;
		ep->height = ZOMBIE_HEIGHT;
		ep->image = zombie;
		ep->alive = TRUE;
		ep->dx = speeds[rand() % nspeeds];
		ep->dy = speeds[rand() % nspeeds];
		ep->direction = ep->dx > 0 ? RIGHT : LEFT;

		// Make sure enemies don't appear inside an obstacle
		while (checkHitBounds(ep) || checkObstacleCollision(ep) 
			|| collides(ep->x, ep->y, ep->width, ep->height, 0, 0, player1.width + 3, player1.height + 3)) {
				ep->x = rand() % ENEMY_COL_CONSTRAINT;
				ep->y = rand() % ENEMY_ROW_CONSTRAINT;
		}
	}

	// Build new weapon for each level
	weapon.x = rand() % WEAPON_COL_CONSTRAINT;
	weapon.y = rand() % WEAPON_ROW_CONSTRAINT;
	weapon.width = BLASTER_WIDTH;
	weapon.height = BLASTER_HEIGHT;
	weapon.available = TRUE;
	weapon.image = blaster;
	weapon.shotBullets = 0;

	// Make sure the weapon doesn't appear inside an obstacle
	while (checkHitBounds(&weapon) || checkObstacleCollision(&weapon)
		|| collides(weapon.x, weapon.y, weapon.width, weapon.height, 0, 0, player1.width + 3, player1.height + 3)) {
			weapon.x = rand() % WEAPON_COL_CONSTRAINT;
			weapon.y = rand() % WEAPON_ROW_CONSTRAINT;
	}
}

/* Drawing */
/* drawStartScreen
*	Creates a start menu with game instructions and player prompts
*	to start
*/
void drawStartScreen() {
	drawImage3(0, 0, HEADER_WIDTH, HEADER_HEIGHT, header);
	drawString(30, 72, "ZOMBIE APOCALYPSE", WHITE);
	drawString(45, 63, "1. Grab the weapon.", WHITE);
	drawString(60, 45, "2. Defeat all the zombies.", WHITE);
	drawString(135, 62, "Press START to Play!", WHITE);
}

/* initializeNextLevel
* 	Creates the new level variables
*	Draws the new level scene
*/
void initializeNextLevel() {
	initializeGameVariables();
	drawLevelScene();
}

/* drawLevelScene
*	Draws the status bar
*	Draws the weapon
*	Draws the enemies
*	Draws the obstacles
*/
void drawLevelScene() {
	// Draw background imagery
	drawFilledRectangle(0, 0, GBA_SCREEN_WIDTH, GBA_SCREEN_HEIGHT, backgroundColor);
	
	// Draw Status Bar
	drawFilledRectangle(GBA_SCREEN_WIDTH, GBA_SCREEN_HEIGHT - 10, GBA_SCREEN_WIDTH, 10, BLACK);

	// Draw weapon
	drawWeapon();

	// Draw enemies
	drawEnemies();

	// Draw obstacles
	drawObstacles();
}

/* drawGameOverScreen
*	Creates a game over screen for when the player loses
*	Prompts player to replay
*	Shows player's high score
*/
void drawGameOverScreen() {
	// Draw the screen and prompt
	drawImage3(0, 0, GAMEOVER_WIDTH, GAMEOVER_HEIGHT, gameover);
	drawString(130, 60, "Press START to replay", WHITE);

	// Compute and show the high score
	char scoreStr[20];
	score += player1.level * (MAX_BULLETS - weapon.shotBullets) * player1.kills + 5 * player1.lives;
	sprintf(scoreStr, "Score: %d", score);
	drawString(145, 93, scoreStr, WHITE);
}

/* drawGameOverWinScreen
*	Creates a game over screen for when the player wins
*	Prompts the user to replay
*	Shows the player's high score
*/
void drawGameOverWinScreen() {
	// Draw the screen and prompt
	drawImage3(0, 0, YOUWIN_WIDTH, YOUWIN_HEIGHT, youwin);
	drawString(130, 65, "Press START to replay", WHITE);

	// Compute and show the high score
	char scoreStr[20];
	score += player1.level * (MAX_BULLETS - weapon.shotBullets) * player1.kills + 100 * player1.lives;
	sprintf(scoreStr, "Score: %d", score);
	drawString(145, 93, scoreStr, WHITE);
}

/* drawEnemies
*	Draws all the enemies needed for the current level
*	Clears the trail of the enemy's last movement
*	Draws the updates enemy depending on which direction it is facing
*/
void drawEnemies() {
	for (int i = 0; i < nEnemiesInLevel; ++i) {
		Enemy* ep = &(enemies[i]);
		if (ep->alive) {
			// Clear this enemy's trail
			if (ep->oldPosX != ep->x || ep->oldPosY != ep->y) {
				drawFilledRectangle(ep->oldPosX, ep->oldPosY, ep->width, ep->height, backgroundColor);
			}

			// Draw the updated position of this enemy
			if (ep->direction == RIGHT) {
				drawImage3(ep->y, ep->x, ep->width, ep->height, ep->image);
			} else {
				drawImageFlippedLR(ep->y, ep->x, ep->width, ep->height, ep->image);
			}
		}
	}
}

/* drawObstacles
*	Draws all the obstacles needed for the current level
*/
void drawObstacles() {
	for (int i = 0; i < nObstaclesInLevel; ++i) {
		Obstacle* op = &(obstacles[i]);
		drawImage3(op->y, op->x, op->width, op->height, op->image);
	}
}

/* drawWeapon
*	Draws the weapon if it is available
*/
void drawWeapon() {
	if (weapon.available) {	
		drawImage3(weapon.y, weapon.x, weapon.width, weapon.height, weapon.image);
	}
}

/* drawBullets
*	Draws all the active bullets on the screen
*/
void drawBullets() {
	for (int i = 0; i < weapon.shotBullets; ++i) {
		Bullet* bp = &(weapon.bullets[i]);
		// Only update active bullets
		if (bp->active) {
			// Clear this bullet's trail
			if (bp->oldPosX != bp->x || bp->oldPosY != bp->y) {
				drawFilledRectangle(bp->oldPosX, bp->oldPosY, bp->width, bp->height, backgroundColor);
			}

			// Draw the updated position of this bullet
			drawFilledRectangle(bp->x, bp->y, bp->width, bp->height, MAGENTA);
		}
	}
}

/* updateScreen
*	Handles all the drawing during each iteration of the loop
*	Redraws all the updated objects on the screen
*/
void updateScreen() {
	// Draw the status bar
	if (readyToUpdateStatusBar) {
		drawFilledRectangle(GBA_SCREEN_WIDTH, GBA_SCREEN_HEIGHT - 10, GBA_SCREEN_WIDTH, 10, BLACK);

		// Draw the level details in the status bar
		// lives
		char livesStr[10];
		sprintf(livesStr, "Lives: %d", player1.lives);
		drawString(GBA_SCREEN_HEIGHT - 8, 0, livesStr, WHITE);

		// ammo
		char ammoStr[10];
		sprintf(ammoStr, "Ammo: %d", MAX_BULLETS - 1 - weapon.shotBullets);
		drawString(GBA_SCREEN_HEIGHT - 8, GBA_SCREEN_WIDTH / 2 - 20, player1.hasWeapon ? ammoStr : "Ammo: N/A", WHITE);

		// kills
		char killsStr[10];
		sprintf(killsStr, "Kills: %d", player1.kills);
		drawString(GBA_SCREEN_HEIGHT - 8, GBA_SCREEN_WIDTH - 50, killsStr, WHITE);
	
		// Don't update continuously unless these variables change for efficiency
		readyToUpdateStatusBar = FALSE;
	}

	// Draw weapon
	if (weapon.available) {
		drawImage3(weapon.y, weapon.x, weapon.width, weapon.height, weapon.image);
	}

	// Draw enemies
	drawEnemies();

	// Draw obstacles
	drawObstacles();

	// Draw bullets
	drawBullets();

	// Clear the player's trail
	if (player1.oldPosX != player1.x || player1.oldPosY != player1.y) {
		drawFilledRectangle(player1.oldPosX, player1.oldPosY, player1.width, player1.height, backgroundColor);
	}
	// Draw the updated position of the player
	drawImage3(player1.y, player1.x, player1.width, player1.height, player1.image);
}

/* Object Control */
/* movePlayer
*	Updates the player properties and conditions
*	NOTE: There is no drawing handled by this method
*	Drawing the player is handled in the updateScreen method
*/
void movePlayer() {
	// Update the player's last position
	player1.oldPosX = player1.x;
	player1.oldPosY = player1.y;

	// Get user input to control the player
	if (KEY_DOWN_NOW(BUTTON_UP)) {
		player1.y -= player1.speed;
	}
	if (KEY_DOWN_NOW(BUTTON_DOWN)) {
		player1.y += player1.speed;
	}
	if (KEY_DOWN_NOW(BUTTON_LEFT)) {
		player1.x -= player1.speed;
	}
	if (KEY_DOWN_NOW(BUTTON_RIGHT)) {
		player1.x += player1.speed;
	}

	// Keep the player in the GBA bounds
	checkHitBounds(&player1);

	// Keep the player from hitting obstacles
	checkObstacleCollision(&player1);

	// Grab the weapon if the player moves over it
	grabWeapon();

	// Lose a life if player collides with an enemy
	dieFromEnemy();

	// Shoot the weapon if the player has it equipped
	shoot();
}

/* dieFromEnemy
*	Conditional for player
*	Requires status bar update (handled in updateScreen)
*	If an enemy collides with the player, the player:
*		1. Loses a life
*		2. Returns to the spawn position
*/
void dieFromEnemy() {
	// Cycle through all enemies in the current level
	for (int i = 0; i < nEnemiesInLevel; ++i) {
		Enemy* ep = &(enemies[i]);
		// Only consider enemies that are still alive
		if (ep->alive) {
			if (collides(player1.x, player1.y, player1.width, player1.height, ep->x, ep->y, ep->width, ep->height)) {
				// Clear the player's position of death
				drawFilledRectangle(player1.x, player1.y, player1.width, player1.height, backgroundColor);
				
				// Update the relevant properties
				player1.lives--;
				player1.x = 0;
				player1.y = 0;

				// Indicate that the status bar should be updated
				readyToUpdateStatusBar = TRUE;
			}
		}
	}
}

/* grabWeapon
*	Conditional for player
* 	Requires status bar update (handled in updateScreen)
*	If the player collides with the [active] weapon:
*		1. The player can shoot up to 100 bullets
*		2. The weapon disappears from the screen 
*/
void grabWeapon() {
	if (weapon.available && collides(player1.x, player1.y, player1.width, player1.height, weapon.x, weapon.y, weapon.width, weapon.height)) {
		// Update the relevant properties
		weapon.available = FALSE;
		player1.hasWeapon = TRUE;

		// Clear the weapon's position of pickup
		drawFilledRectangle(weapon.x, weapon.y, weapon.width, weapon.height, backgroundColor);
		
		// Indicate that the status bar should be updated
		readyToUpdateStatusBar = TRUE;
	}
}

/* shoot
*	Action for player
*	Requires status bar update (handled in updateScreen)
*	Shoots a bullet in the direction of the button pressed:
*		'A' - UP
*		'B' - DOWN
*		'L' - LEFT
*		'R' - RIGHT
*	Prevents player from holding down a button and spamming
*	bullets
*/
void shoot() {
	if (player1.hasWeapon && weapon.shotBullets < MAX_BULLETS) {
		// Shoot Up
		if (KEY_DOWN_NOW(BUTTON_A) && !a_down) {
			a_down = TRUE;
			// Create a new bullet
			Bullet b1 = {player1.x + PLAYER_WIDTH / 2, player1.y + PLAYER_HEIGHT / 2, 5, 5, 0, 0, UP, TRUE };
			
			// Put this bullet in the bullet array, update ammo
			weapon.bullets[weapon.shotBullets++] = b1;

			// Indicate that the status bar should be updated
			readyToUpdateStatusBar = TRUE;
		}
		// Prevent spamming from holding down a key
		if (!KEY_DOWN_NOW(BUTTON_A && a_down)) {
			a_down = FALSE;
		}

		// Shoot Down
		if (KEY_DOWN_NOW(BUTTON_B) && !b_down) {
			b_down = TRUE;
			Bullet b2 = {player1.x + PLAYER_WIDTH / 2, player1.y + PLAYER_HEIGHT / 2, 5, 5, 0, 0, DOWN, TRUE };
			weapon.bullets[weapon.shotBullets++] = b2;
			readyToUpdateStatusBar = TRUE;
		}
		if (!KEY_DOWN_NOW(BUTTON_B) && b_down) {
			b_down = FALSE;
		}

		// Shoot Left
		if (KEY_DOWN_NOW(BUTTON_L) && !l_down) {
			l_down = TRUE;
			Bullet b3 = {player1.x + PLAYER_WIDTH / 2, player1.y + PLAYER_HEIGHT / 2, 5, 5, 0, 0, LEFT, TRUE };
			weapon.bullets[weapon.shotBullets++] = b3;
			readyToUpdateStatusBar = TRUE;
		}
		if (!KEY_DOWN_NOW(BUTTON_L) && l_down) {
			l_down = FALSE;
		}

		// Shoot Right
		if (KEY_DOWN_NOW(BUTTON_R) && !r_down) {
			r_down = TRUE;
			Bullet b4 = {player1.x + PLAYER_WIDTH / 2, player1.y + PLAYER_HEIGHT / 2, 5, 5, 0, 0, RIGHT, TRUE };
			weapon.bullets[weapon.shotBullets++] = b4;
			readyToUpdateStatusBar = TRUE;
		}
		if (!KEY_DOWN_NOW(BUTTON_R) && r_down) {
			r_down = FALSE;
		}
	}
}

/* moveBullet
*	Updates each active bullet's properties and conditions
*	NOTE: There is no drawing handled by this method
*	Drawing the bullets is handled in the updateScreen method
*/
void moveBullet() {
	for (int i = 0; i < weapon.shotBullets; ++i) {
		Bullet* bp = &(weapon.bullets[i]);
		if (bp->active) {
			// Save the bullet's last position
			bp->oldPosX = bp->x;
			bp->oldPosY = bp->y;

			// Move the bullet in its shot direction
			switch (bp->direction) {
				case UP:
					bp->y -= 5;
					break;
				case DOWN:
					bp->y += 5;
					break;
				case LEFT:
					bp->x -= 5;
					break;
				case RIGHT:
					bp->x += 5;
					break;
			}

			// Check if this bullet has become inactive
			if (checkHitBounds(bp) || checkBulletHitEnemy(bp)
				|| checkBulletHitObstacle(bp)) {
				bp->active = FALSE;
			}

			// If this bullet is inactive, clear its position
			if (!bp->active) {
				drawFilledRectangle(bp->oldPosX, bp->oldPosY, bp->width, bp->height, backgroundColor);
			}
		}
	}
}

/* moveEnemies
*	Updates each alive enemy's properties and conditions
*	NOTE: There is no drawing handled by this method
*	Drawing the enemies is handled in the updateScreen method
*/
void moveEnemies() {
	for (int i = 0; i < nEnemiesInLevel; ++i) {
		Enemy* ep = &(enemies[i]);
		if (ep->alive) {
			// Update this enemy's last position
			ep->oldPosX = ep->x;
			ep->oldPosY = ep->y;

			// Move this enemy automatically
			ep->x += ep->dx;
			ep->y += ep->dy;

			// Check if this enemy needs to change direction
			if (checkHitBounds(ep) || checkObstacleCollision(ep)
				|| checkEnemyOnEnemyCollision(i, ep)) {
				// Randomize the new direction of the 
				switch (rand() % 3) {
					case 0:
						ep->dx = -ep->dx;
						ep->dy = -ep->dy;
						break;
					case 1: 
						ep->dx = -ep->dx;
						break;
					case 2: 
						ep->dy = -ep->dy;
						break;
				}
				ep->direction = ep->dx > 0 ? RIGHT : LEFT;
			}
		}
	}
}

/* Object Conditions */
/* collides
*	Check if two objects collide
*/
bool collides(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2) {
	int top1, bottom1, left1, right1;
	int top2, bottom2, left2, right2;

	left1 = x1;
	right1 = x1 + width1;
	top1 = y1;
	bottom1 = y1 + height1;

	left2 = x2;
	right2 = x2 + width2;
	top2 = y2;
	bottom2 = y2 + height2;

	return ((bottom1 > top2) && (top1 < bottom2) && (right1 > left2) && (left1 < right2));
}

/* checkHitBounds
*	Check if an object hit the screen bounds
*	NOTE: Corrects the object's position to hit the boundary 
*	instead of passing it
*	NOTE: The status bar is considered the bottom of 
*	the screen (10px from bottom)
*/
bool checkHitBounds(void* o) {
	Object* obj = (Object*) o;
	bool hitBound = FALSE;

	if (obj->x < 0) {
		obj->x = 0;
		hitBound = TRUE;
	}
	if (obj->x + obj->width > GBA_SCREEN_WIDTH) {
		obj->x = GBA_SCREEN_WIDTH - obj->width;
		hitBound = TRUE;
	}
	if (obj->y < 0) {
		obj->y = 0;
		hitBound = TRUE;
	}
	if (obj->y + obj->height > GBA_SCREEN_HEIGHT - 10) {
		obj->y = GBA_SCREEN_HEIGHT - obj->height - 9;
		hitBound = TRUE;
	}

	return hitBound;
}

/* checkObstacleCollision
*	Checks if an object collides with an obstacle
*	NOTE: Corrects the object's position to hit the obstacle
*	instead of passing through it
*/
bool checkObstacleCollision(void* o) {
	Object* obj = (Object*) o;
	bool hitObstacle = FALSE;

	// Cycle through all the obstacles
	for (int i = 0; i < nObstaclesInLevel; ++i) {
		Obstacle* op = &(obstacles[i]);

		if (collides(obj->x, obj->y, obj->width, obj->height, op->x, op->y, op->width, op->height)) {
			// Correct the object's position
			obj->x = obj->oldPosX;
			obj->y = obj->oldPosY;

			// Indicate that an obstacle has been hit
			hitObstacle = TRUE;
		}
	}

	return hitObstacle;
}

/* checkEnemyOnEnemyCollision
*	Checks if the 'index'th active enemy collides with another enemy
*	NOTE: This method also accounts for enemies avoiding the player spawn point
*/
bool checkEnemyOnEnemyCollision(int index, void* o) {
	Object* obj = (Object*) o;
	bool hitEnemy = FALSE;

	// Cycle through all the enemies in the current level (exclude the i == index)
	for (int i = 0; i < nEnemiesInLevel; ++i) {
		if (i != index) {
			Enemy* ep = &(enemies[i]);
			// Only consider alive enemies
			if (ep->alive) {
				if (collides(obj->x, obj->y, obj->width, obj->height, ep->x, ep->y, ep->width, ep->height)) {
					// Indicate that an enemy has been hit
					hitEnemy = TRUE;
				}
			}
		}
	}

	// Ensure this enemy avoids the player spawn point as if it hit another enemy
	Enemy* thisEp = &(enemies[index]);
	if (thisEp->alive) {
		if (collides(thisEp->x, thisEp->y, thisEp->width, thisEp->height, 0, 0, player1.width + 5, player1.height + 5)) {
			hitEnemy = TRUE;
		}
	}

	return hitEnemy;
}

/* checkBulletHitEnemy
* 	Checks if a bullet has hit an enemy
*	If a bullet does hit an enemy:
*		1. The player gets another kill
*		2. The enemy dies
*/
bool checkBulletHitEnemy(Bullet* bp) {
	bool hitEnemy = FALSE;

	// Cycle through all the enemies in the current level
	for (int i = 0; i < nEnemiesInLevel; ++i) {
		Enemy* ep = &(enemies[i]);
		// Only consider alive enemies
		if (ep->alive) {
			if (collides(bp->x, bp->y, bp->width, bp->height, ep->x, ep->y, ep->width, ep->height)) {
				// Increase player kills
				player1.kills++;

				// Update the enemy that was hit
				ep->alive = FALSE;
				hitEnemy = TRUE;
				drawFilledRectangle(ep->oldPosX, ep->oldPosY, ep->width, ep->height, backgroundColor);
				
				// Indicate that the status bar needs to be updated
				readyToUpdateStatusBar = TRUE;
			}
		}
	}

	return hitEnemy;
}

/* checkBulletHitObstacle
*	Conditional function to determine if the input bullet
*	collides with any active obstacle
*/
bool checkBulletHitObstacle(Bullet* bp) {
	bool hitObstacle = FALSE;

	// Cycle through all active obstacles to check if the bullet has hit any
	for (int i = 0; i < nObstaclesInLevel; ++i) {
		Obstacle* op = &(obstacles[i]);
		if (collides(bp->x, bp->y, bp->width, bp->height, op->x, op->y, op->width, op->height)) {
			hitObstacle = TRUE;
		}
	}

	return hitObstacle;
}

/* Game Execution */
/* execute
*	This method is the bread and butter of the game; it is run
*	every loop cycle when the user is in a level
*	All object updating is done by the move[Object] methods
*	All drawing is done by the updateScreen method
*/
void execute() {
	moveEnemies();
	movePlayer();
	moveBullet();
	updateScreen();
}

/* reset
*	Resets the player variables to prepare for a new game
*	This method resets the variables that are not already
*	reset by the initializeGameVariables method
*/
void reset() {
	// Clear the score
	score = 0;

	// Reset player1 members
	player1.level = 1;
	player1.lives = 3;
	player1.hasWeapon = FALSE;

	// Reset weapon members
	weapon.available = TRUE;
}

/* pause
*	Pauses the game while playing
* 	Pulls up the pause menu which allows the user to:
*		1. Resume playing (START)
*		2. Exit to the start menu (SELECT)
*/
void pause() {
	// Draw the menu
	drawFilledRectangle(50, 55, 150, 65, BLACK);
	drawString(65, 105, "PAUSED", WHITE);
	drawString(80, 60, "Press START to resume", WHITE);
	drawString(95, 62, "Press SELECT to quit", WHITE);

	// Store the current level
	paused_level = player1.level;	
}

/* resume
*	Resumes playing after pausing the game
*/
void resume() {
	readyToUpdateStatusBar = TRUE;
	drawLevelScene();
}

/* Game State Conditions */
/* checkKills
*	The player moves on to the next level if all
*	the enemies in that level have been killed
*/
bool checkKills() {
	return player1.kills == nEnemiesInLevel;
}

/* checkGameOver
*	The game ends based on two conditions:
*		1. The player runs out of lives
*		2. The player runs out of ammo in a single level
*/
bool checkGameOver() {
	return player1.lives <= 0 || weapon.shotBullets == MAX_BULLETS - 1;
}

