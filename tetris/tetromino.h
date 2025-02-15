#ifndef APP_TETRIS_SPRITES_H
#define APP_TETRIS_SPRITES_H

#include "/datatypes.h"

/*
 * Tetrominos are rendered with lo-res simple sprites.
 * Four sprites are reserved and initialised so that they
 * are four bricks tall, making a 4x4 grid.
 * A tetromino is then rendered using MoveSprite for each,
 * with selected starting points and heights.
 */
#define NUM_SPRITES 4
#define NUM_COLUMNS 4
#define NUM_ROWS 4
#define NUM_ORIENTATIONS 4
#define BRICK_WIDTH 16
#define BRICK_HEIGHT 16
#define BRICK_DEPTH 2
#define BRICK_LENGTH BRICK_DEPTH * BRICK_HEIGHT
#define BRICK_SIZE sizeof(UWORD) * BRICK_LENGTH
#define SPRITE_LENGTH BRICK_LENGTH * NUM_ROWS
#define VP_X_FACTOR 32 //hires
#define VP_Y_FACTOR 32 //lace

typedef struct
{
	UWORD posctl[2];
	UWORD data[SPRITE_LENGTH];
	UWORD reserved[2];
} SpriteImage;

typedef enum
{
	T, Name_sizeof
} Name;

typedef struct
{
	UBYTE start, height;
} Column;

typedef struct
{
	BYTE left, top, right, bottom;
} Box;

typedef struct
{
	Column c[NUM_COLUMNS];
	Box b;
} Orientation;

typedef struct
{
	Orientation o[NUM_ORIENTATIONS];
} TetrominoShapes;

typedef struct
{
	BYTE c, r;
} Position;

typedef struct
{
	Position p;
	UBYTE t;
	UBYTE o;
} Tetromino;

BOOL init_tetrominos(ViewPort*, PictureData*);
VOID free_sprites(VOID);
VOID render_tetromino(VOID);
VOID render_all_sprites(VOID); // debug purpose

extern Tetromino current;
extern TetrominoShapes tetromino[];

#endif
