/*@file objects.h
 *@author Nick Liccini
 */

#ifndef OBJECTS_H
#define OBJECTS_H

/* enums and typedefs */
enum Direction {
	RIGHT,
	LEFT,
	UP,
	DOWN
};

typedef struct {
	int x;
	int y;
	int width;
	int height;
	int oldPosX;
	int oldPosY;
} Object;

typedef struct {
	int x;
	int y;
	int width;
	int height;
	int oldPosX;
	int oldPosY;
	int lives;
	bool hasWeapon;
	int speed;
	int level;
	int kills;
	const u16* image;
} Player;

typedef struct {
	int x;
	int y;
	int width;
	int height;
	int oldPosX;
	int oldPosY;
	bool alive;
	int dx;
	int dy;
	enum Direction direction;
	const u16* image;
} Enemy;

typedef struct {
	int x;
	int y;
	int width;
	int height;
	int oldPosX;
	int oldPosY;
	enum Direction direction;
	bool active;
} Bullet;

typedef struct {
	int x;
	int y;
	int width;
	int height;
	bool available;
	int shotBullets;
	Bullet bullets[100];
	const u16* image;
} Weapon;


typedef struct {
	int x;
	int y;
	int width;
	int height;
	const u16* image;
} Obstacle;

#endif