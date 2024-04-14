#ifndef GAME_TETRIS
#define GAME_TETRIS

#define NUM_SPRITES 4
#define BRICK_HEIGHT 16
#define SPRITE_DEPTH 2
#define BRICK_LEN (SPRITE_DEPTH * BRICK_HEIGHT)
#define BRICK_SIZ (sizeof(UWORD) * BRICK_LEN)

#include "datatypes.h"

void init_game(void);
void render_frame(struct ViewPort*, InputState*);

typedef enum
{
	GS_BEFORE,
	GS_PLAY,
	GS_PAUSE,
	GS_AFTER
} GameState;

typedef struct
{
  UBYTE what; // 7 shapes in 4 rotational states
} Tetramino;

typedef BYTE Brick; // <0 is empty cell

typedef struct
{
	UWORD posctl[2];
	UWORD data[BRICK_LEN];
	UWORD reserved[2];
} BrickImageH1;

typedef struct
{
	UWORD posctl[2];
	UWORD data[2 * BRICK_LEN];
	UWORD reserved[2];
} BrickImageH2;

typedef struct
{
	UWORD posctl[2];
	UWORD data[3 * BRICK_LEN];
	UWORD reserved[2];
} BrickImageH3;

typedef struct
{
	UWORD posctl[2];
	UWORD data[4 * BRICK_LEN];
	UWORD reserved[2];
} BrickImageH4;

typedef struct
{
  Brick grid[17][10];
} Playground;

typedef struct
{
  UWORD speed;
  UWORD bonus;
} Level;

typedef struct
{
  UWORD hiscore;
  UWORD score;
} Gamestat;

#endif
