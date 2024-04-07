#ifndef GAME_TETRIS
#define GAME_TETRIS

#include "datatypes.h"

void render_frame(InputState*);

typedef enum
{
	GS_BEFORE,
	GS_PLAY,
	GS_PAUSE,
	GS_AFTER
} GameState;

typedef struct
{
  UBYTE row[4]; // 7 shapes in 4 rotational states
} Tetramino;

typedef BYTE Brick; // <0 is empty cell

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
