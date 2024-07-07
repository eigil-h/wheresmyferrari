#ifndef GAME_TETRIS
#define GAME_TETRIS

#include <devices/timer.h>
#include "datatypes.h"

#define NUM_SPRITES 4
#define BRICK_HEIGHT 16
#define BRICK_WIDTH 16
#define SPRITE_DEPTH 2
#define BRICK_LEN (SPRITE_DEPTH * BRICK_HEIGHT)
#define BRICK_SIZ (sizeof(UWORD) * BRICK_LEN)
#define PLG_WIDTH 10
#define PLG_HEIGHT 13

VOID init_game(ViewPort*);
VOID render_frame(InputState*);
ViewRequest* prepare_gfx(PictureData* bg, PictureData* brick, Error*);


typedef enum
{
	GS_BEFORE,
	GS_PLAY,
	GS_PAUSE,
	GS_AFTER
} GameState;

typedef void (*TetraminoRenderer)(UBYTE x, UBYTE y);

typedef BYTE Brick; // <0 is empty cell

typedef struct
{
	UWORD posctl[2];
	UWORD data[4 * BRICK_LEN];
	UWORD reserved[2];
} BrickImage;

typedef struct
{
	BYTE x;
	UBYTE y;
} Position;

typedef struct
{
	UBYTE start;
	UBYTE height;
} TetrominoColumn;

typedef struct
{
	TetrominoColumn c[4];
	UWORD first, last;
} TetrominoOrientation;

typedef struct
{
	TetrominoOrientation o[4];
} TetrominoData;

typedef struct
{
	Position p;
	UBYTE t;
	UBYTE o;
} Tetromino;

typedef struct
{
  Brick grid[PLG_HEIGHT][PLG_WIDTH];
} Playground;

typedef struct
{
  TimeVal_Type speed;
  ULONG score;
} Level;

typedef struct
{
  ULONG hiscore;
  ULONG score;
  UWORD level;
  UWORD pad;
} Game;

#endif
