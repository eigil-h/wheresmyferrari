#ifndef APP_GAME_TETRIS
#define APP_GAME_TETRIS

#include <devices/timer.h>
#include "/datatypes.h"

#define CELL_WIDTH 32
#define CELL_HEIGHT 32
#define PLAYGROUND_COLUMNS 10
#define PLAYGROUND_ROWS 13

typedef enum
{
	GS_BEFORE,
	GS_PLAY,
	GS_PAUSE,
	GS_AFTER
} GameState;

typedef void (*TetrominoRenderer)(UBYTE x, UBYTE y);
typedef BYTE Cell; // <0 is empty cell

typedef struct
{
  Cell grid[PLAYGROUND_ROWS][PLAYGROUND_COLUMNS];
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

VOID init_game(ViewPort*);
VOID render_frame(InputState*);

#endif
