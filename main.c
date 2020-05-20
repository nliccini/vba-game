/*@file main.c
 *@author Nick Liccini
 */

#include "myLib.h"
#include "text.h"
#include "game.h"

enum GameState {
	STATE_INIT,
	STATE_START_SCREEN,
	STATE_LEVEL_1,
	STATE_LEVEL_2,
	STATE_LEVEL_3,
	STATE_PAUSE,
	STATE_GAME_OVER,
	STATE_GAME_OVER_WIN
};

/* updateStateAsNeeded
*	Updates the state of the game depending on the current game status
*		1. Game Over (player loses)
*		2. Advance to Next Level (player gets all the kills in that level)
*		3. Pause (player presses START during the game)
*/
void updateStateAsNeeded(enum GameState* state, bool* start_down) {
	// 1. Game Over
	if (checkGameOver()) {
		drawGameOverScreen();
		score = 0;
		*state = STATE_GAME_OVER;
	}

	// 2. Level completed
	if (checkKills()) {
		switch (player1.level) {
			case 1: 
				player1.level++;
				score += player1.level * (MAX_BULLETS - weapon.shotBullets) * player1.kills;
				initializeNextLevel();
				*state = STATE_LEVEL_2;
				break;
			case 2:
				player1.level++;
				score += player1.level * (MAX_BULLETS - weapon.shotBullets) * player1.kills;
				initializeNextLevel();
				*state = STATE_LEVEL_3;
				break;
			case 3:
				score += player1.level * (MAX_BULLETS - weapon.shotBullets) * player1.kills;
				drawGameOverWinScreen();
				reset();
				*state = STATE_GAME_OVER_WIN;
				break;
		}
	}

	// 3. Paused
	if (KEY_DOWN_NOW(BUTTON_START) && !(*start_down)) {
		*start_down = TRUE;
		pause();
		*state = STATE_PAUSE;
	}
	if (!KEY_DOWN_NOW(BUTTON_START) && *start_down) {
		*start_down = FALSE;
	}
}

int main() {
	REG_DISPCNT = MODE3 | BG2_ENABLE;

	enum GameState state = STATE_INIT;
	static bool start_down = FALSE;
	unsigned int seedCounter = 0;

	while (1) {
		waitForVblank();

		switch(state) {
			case STATE_INIT:
				// Draw start screen, prompt user to press START
				drawStartScreen();
				state = STATE_START_SCREEN;
				break;

			case STATE_START_SCREEN:
				seedCounter++;
				// Start on level 1 on START
				if (KEY_DOWN_NOW(BUTTON_START) && !start_down) {
					srand(seedCounter);
					start_down = TRUE;
					initializeNextLevel();
					state = STATE_LEVEL_1;
				}
				if (!KEY_DOWN_NOW(BUTTON_START) && start_down) {
					start_down = FALSE;
				}
				break;

			case STATE_LEVEL_1:
				execute();
				updateStateAsNeeded(&state, &start_down);
				break;

			case STATE_LEVEL_2:
				execute();
				updateStateAsNeeded(&state, &start_down);
				break;

			case STATE_LEVEL_3:
				execute();
				updateStateAsNeeded(&state, &start_down);
				break;

			case STATE_PAUSE:
				// Resume on START
				if (KEY_DOWN_NOW(BUTTON_START) && !start_down) {
					start_down = TRUE;
					switch (paused_level) {
						case 1:
							state = STATE_LEVEL_1;
							break;
						case 2:
							state = STATE_LEVEL_2;
							break;
						case 3:
							state = STATE_LEVEL_3;
							break;
					}
					resume();
				}
				if (!KEY_DOWN_NOW(BUTTON_START) && start_down) {
					start_down = FALSE;
				}

				// Exit to start screen on SELECT
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					reset();
					state = STATE_INIT;
				}
				break;

			case STATE_GAME_OVER:
				// Display GAME OVER text and indicate to press start to restart 
				if (KEY_DOWN_NOW(BUTTON_START) && !start_down) {
					start_down = TRUE;
					reset();
					initializeNextLevel();
					state = STATE_LEVEL_1;
				}
				if (!KEY_DOWN_NOW(BUTTON_START) && start_down) {
					start_down = FALSE;
				}

				// Exit to start screen on SELECT
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					reset();
					state = STATE_INIT;
				}
				break;

			case STATE_GAME_OVER_WIN:
				// Display GAME OVER WIN text and indicate to press start to restart 
				if (KEY_DOWN_NOW(BUTTON_START) && !start_down) {
					start_down = TRUE;
					reset();
					initializeNextLevel();
					state = STATE_LEVEL_1;
				}
				if (!KEY_DOWN_NOW(BUTTON_START) && start_down) {
					start_down = FALSE;
				}

				// Exit to start screen on SELECT
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					reset();
					state = STATE_INIT;
				}
				break;
		}

	}

	return 0;
}